#include "KDTree.h"
#include "../Tool/Memory.h"

RENDER_BEGIN

// Represent the leaves and interior node of kd-tree.
// Each union member indicate whether a particular field is used for interior nodes, leaf nodes, or both.
class KdTreeNode
{
public:
	void initLeafNode(int* PrimitiveIndices, int np, std::vector<int>* primitiveIndices);

	void initInteriorNode(int axis, int ac, Float split)
	{
		// Note: we lay the nodes out in a way that lets us only store one child pointer : 
		//		 all of the nodes are allocated in a single contiguous block of
		//		 memory, and the child of an interior node that is responsible for space below the splitting
		//		 plane is always stored in the array position immediately after its parent
		m_split = split;
		m_flags = axis;
		// Note: The other child, representing space above the splitting plane, will end up somewhere else
		//       in the array, and m_rightChildIndex stores its position in the nodes array
		m_rightChildIndex |= (ac << 2);
	}

	Float splitPos() const { return m_split; }
	int numPrimitives() const { return m_nPrimitives >> 2; }
	int splitAxis() const { return m_flags & 3; }
	bool isLeaf() const { return (m_flags & 3) == 3; }
	int aboveChild() const { return m_rightChildIndex >> 2; }

	union
	{
		Float m_split; // Split position which is for interior nodes
		int m_onePrimitive; // Leaf
		int m_PrimitiveIndicesOffset; // Leaf
	};

private:
	union 
	{
		int m_flags; // Both
		int m_nPrimitives; // Leaf
		int m_rightChildIndex; // Interior
	};
};

enum class EdgeType { Start, End };
class BoundEdge
{
public:
	BoundEdge() = default;
	BoundEdge(Float t, int PrimitiveIndex, bool starting) : m_t(t), m_PrimitiveIndex(PrimitiveIndex)
	{
		m_type = starting ? EdgeType::Start : EdgeType::End;
	}

	Float m_t;
	int m_PrimitiveIndex;
	EdgeType m_type;
};

void KdTreeNode::initLeafNode(int* PrimitiveIndices, int np, std::vector<int>* primitiveIndices)
{
	// Note: the low 2 bits of m_flags which holds the value 3 indicate that it's a leaf node
	//       the upper 30 bits of m_nPrims are available to record how many Primitives overlap it.
	m_flags = 3;
	m_nPrimitives |= (np << 2);

	// Store Primitive ids for leaf node
	if (np == 0)
	{
		m_onePrimitive = 0;
	}
	else if (np == 1)
	{
		m_onePrimitive = PrimitiveIndices[0];
	}
	else
	{
		// Note: if more than one primitive overlaps, then their indices are
		//       stored in a segment of m_primitiveIndicesOffset. The offset
		//       to the first index for the leaf is stored in m_primitiveIndicesOffset
		m_PrimitiveIndicesOffset = primitiveIndices->size();
		for (int i = 0; i < np; ++i)
		{
			primitiveIndices->push_back(PrimitiveIndices[i]);
		}
	}
}

KdTree::KdTree(const std::vector<Primitive::ptr>& Primitives, int isectCost/* = 80*/, int traversalCost/* = 1*/,
	Float emptyBonus/* = 0.5*/, int maxPrimitives/* = 1*/, int maxDepth/* = -1*/) :
	m_isectCost(isectCost),
	m_traversalCost(traversalCost),
	m_maxPrimitives(maxPrimitives),
	m_emptyBonus(emptyBonus),
	m_Primitives(Primitives)
{
	m_nextFreeNode = m_nAllocedNodes = 0;

	// The tree cannot grow without bound in pathological cases. (8 + 1.3log(N))
	if (maxDepth <= 0)
	{
		maxDepth = std::round(8 + 1.3f * glm::log2(float(int64_t(m_Primitives.size()))));
	}

	// Compute bounds for kd-tree construction
	std::vector<Bounds3f> PrimitiveBounds;
	PrimitiveBounds.reserve(m_Primitives.size());
	for (const Primitive::ptr& Primitive : m_Primitives)
	{
		Bounds3f b = Primitive->worldBound();
		m_bounds = unionBounds(m_bounds, b);
		PrimitiveBounds.push_back(b);
	}

	// Allocate working memory for kd-tree construction
	std::unique_ptr<BoundEdge[]> edges[3];
	for (int i = 0; i < 3; ++i)
	{
		edges[i].reset(new BoundEdge[2 * m_Primitives.size()]);
	}

	std::unique_ptr<int[]> leftNodeRoom(new int[m_Primitives.size()]);
	std::unique_ptr<int[]> rightNodeRoom(new int[(maxDepth + 1) * m_Primitives.size()]);

	// Initialize _primNums_ for kd-tree construction
	std::unique_ptr<int[]> PrimitiveIndices(new int[m_Primitives.size()]);
	for (size_t i = 0; i < m_Primitives.size(); ++i)
	{
		PrimitiveIndices[i] = i;
	}

	// Start recursive construction of kd-tree
	buildTree(0, m_bounds, PrimitiveBounds, PrimitiveIndices.get(), m_Primitives.size(),
		maxDepth, edges, leftNodeRoom.get(), rightNodeRoom.get());
}

void KdTree::buildTree(int nodeIndex, 
	const Bounds3f& nodeBounds,
	const std::vector<Bounds3f>& allPrimitiveBounds,
	int* PrimitiveIndices, 
	int nPrimitives,
	int depth,
	const std::unique_ptr<BoundEdge[]> edges[3],
	int* leftNodeRoom, 
	int* rightNodeRoom, 
	int badRefines)
{
	CHECK_EQ(nodeIndex, m_nextFreeNode);

	// Get next free node from _nodes_ array
	if (m_nextFreeNode == m_nAllocedNodes)
	{
		// Note: If all of the allocated nodes have been used up, node memory
		//		 is reallocated with twice as many entries and the old values are copied.
		int nNewAllocNodes = glm::max(2 * m_nAllocedNodes, 512);
		KdTreeNode* n = AllocAligned<KdTreeNode>(nNewAllocNodes);
		if (m_nAllocedNodes > 0)
		{
			memcpy(n, m_nodes, m_nAllocedNodes * sizeof(KdTreeNode));
			FreeAligned(m_nodes);
		}
		m_nodes = n;
		m_nAllocedNodes = nNewAllocNodes;
	}
	++m_nextFreeNode;

	// Initialize leaf node if termination criteria met
	if (nPrimitives <= m_maxPrimitives || depth == 0)
	{
		m_nodes[nodeIndex].initLeafNode(PrimitiveIndices, nPrimitives, &m_PrimitiveIndices);
		return;
	}

	// Initialize interior node and continue recursion

	// Choose split axis position for interior node
	int bestAxis = -1, bestOffset = -1;
	Float bestCost = Infinity;
	Float oldCost = m_isectCost * Float(nPrimitives);

	// Current node surface area
	const Float invTotalSA = 1 / nodeBounds.surfaceArea();
	Vector3f diagonal = nodeBounds.m_pMax - nodeBounds.m_pMin;

	// Choose which axis to split along
	int axis = nodeBounds.maximumExtent();
	int retries = 0;

retrySplit:
	// Initialize edges for _axis_
	for (int i = 0; i < nPrimitives; ++i)
	{
		int hi = PrimitiveIndices[i];
		const Bounds3f& bounds = allPrimitiveBounds[hi];
		edges[axis][2 * i] = BoundEdge(bounds.m_pMin[axis], hi, true);
		edges[axis][2 * i + 1] = BoundEdge(bounds.m_pMax[axis], hi, false);
	}

	// Sort _edges_ for _axis_
	std::sort(&edges[axis][0], &edges[axis][2 * nPrimitives],
		[](const BoundEdge& e0, const BoundEdge& e1) -> bool
		{
			if (e0.m_t == e1.m_t)
			{
				return (int)e0.m_type < (int)e1.m_type;
			}
			else
			{
				return e0.m_t < e1.m_t;
			}
		});

	// Compute cost of all splits for _axis_ to find best
	int nBelow = 0, nAbove = nPrimitives;
	const auto& currentEdge = edges[axis];
	for (int i = 0; i < 2 * nPrimitives; ++i)
	{
		if (currentEdge[i].m_type == EdgeType::End)
			--nAbove;
		Float edgeT = currentEdge[i].m_t;
		if (edgeT > nodeBounds.m_pMin[axis] && edgeT < nodeBounds.m_pMax[axis])
		{
			// Compute cost for split at _i_th edge

			// Compute child surface areas for split at _edgeT_
			int otherAxis0 = (axis + 1) % 3, otherAxis1 = (axis + 2) % 3;
			Float belowSA = 2 * (diagonal[otherAxis0] * diagonal[otherAxis1] + (edgeT - nodeBounds.m_pMin[axis]) *
				(diagonal[otherAxis0] + diagonal[otherAxis1]));
			Float aboveSA = 2 * (diagonal[otherAxis0] * diagonal[otherAxis1] + (nodeBounds.m_pMax[axis] - edgeT) *
				(diagonal[otherAxis0] + diagonal[otherAxis1]));
			Float pBelow = belowSA * invTotalSA;
			Float pAbove = aboveSA * invTotalSA;
			Float eb = (nAbove == 0 || nBelow == 0) ? m_emptyBonus : 0;
			Float cost = m_traversalCost + m_isectCost * (1 - eb) * (pBelow * nBelow + pAbove * nAbove);

			// Update best split if this is lowest cost so far
			if (cost < bestCost)
			{
				bestCost = cost;
				bestAxis = axis;
				bestOffset = i;
			}
		}
		if (currentEdge[i].m_type == EdgeType::Start)
			++nBelow;
	}

	DCHECK(nBelow == nPrimitives && nAbove == 0);

	if (bestAxis == -1 && retries < 2)
	{
		++retries;
		axis = (axis + 1) % 3;
		goto retrySplit;
	}

	// Create leaf if no good splits were found
	if (bestCost > oldCost)
		++badRefines;
	if ((bestCost > 4 * oldCost && nPrimitives < 16) || bestAxis == -1 || badRefines == 3)
	{
		m_nodes[nodeIndex].initLeafNode(PrimitiveIndices, nPrimitives, &m_PrimitiveIndices);
		return;
	}

	// Classify primitives with respect to split
	int lnPrimitives = 0, rnPrimitives = 0;
	for (int i = 0; i < bestOffset; ++i)
	{
		if (edges[bestAxis][i].m_type == EdgeType::Start)
			leftNodeRoom[lnPrimitives++] = edges[bestAxis][i].m_PrimitiveIndex;
	}
	for (int i = bestOffset + 1; i < 2 * nPrimitives; ++i)
	{
		if (edges[bestAxis][i].m_type == EdgeType::End)
			rightNodeRoom[rnPrimitives++] = edges[bestAxis][i].m_PrimitiveIndex;
	}

	// Recursively initialize children nodes
	Float tSplit = edges[bestAxis][bestOffset].m_t;
	Bounds3f bounds0 = nodeBounds, bounds1 = nodeBounds;
	bounds0.m_pMax[bestAxis] = bounds1.m_pMin[bestAxis] = tSplit;

	// below subtree node
	buildTree(nodeIndex + 1, bounds0, allPrimitiveBounds, leftNodeRoom, lnPrimitives, depth - 1, edges,
		leftNodeRoom, rightNodeRoom + nPrimitives, badRefines);
	int aboveChildIndex = m_nextFreeNode;

	m_nodes[nodeIndex].initInteriorNode(bestAxis, aboveChildIndex, tSplit);

	// above subtree node
	buildTree(aboveChildIndex, bounds1, allPrimitiveBounds, rightNodeRoom, rnPrimitives, depth - 1, edges,
		leftNodeRoom, rightNodeRoom + nPrimitives, badRefines);
}

KdTree::~KdTree() 
{ 
	FreeAligned(m_nodes); 
}

bool KdTree::hit(const Ray& ray) const
{
	// Compute initial parametric range of ray inside kd-tree extent
	Float tMin, tMax;
	if (!m_bounds.hit(ray, tMin, tMax))
	{
		return false;
	}

	// Prepare to traverse kd-tree for ray
	Vector3f invDir(1 / ray.m_dir.x, 1 / ray.m_dir.y, 1 / ray.m_dir.z);
	constexpr int maxTodo = 64;
	KdToDo todo[maxTodo];
	int todoPos = 0;
	const KdTreeNode* currNode = &m_nodes[0];
	while (currNode != nullptr)
	{
		if (currNode->isLeaf())
		{
			// Check for shadow ray intersections inside leaf node
			int nPrimitives = currNode->numPrimitives();
			if (nPrimitives == 1)
			{
				const Primitive::ptr& p = m_Primitives[currNode->m_onePrimitive];
				if (p->hit(ray))
				{
					return true;
				}
			}
			else
			{
				for (int i = 0; i < nPrimitives; ++i)
				{
					int PrimitiveIndex = m_PrimitiveIndices[currNode->m_PrimitiveIndicesOffset + i];
					const Primitive::ptr& p = m_Primitives[PrimitiveIndex];
					if (p->hit(ray))
					{
						return true;
					}
				}
			}

			// Grab next node to process from todo list
			if (todoPos > 0)
			{
				currNode = todo[--todoPos].node;
				tMin = todo[todoPos].tMin;
				tMax = todo[todoPos].tMax;
			}
			else
			{
				break;
			}
		}
		else
		{
			// Process kd-tree interior node

			// Compute parametric distance along ray to split plane
			int axis = currNode->splitAxis();
			Float tPlane = (currNode->splitPos() - ray.m_origin[axis]) * invDir[axis];

			// Get node children pointers for ray
			const KdTreeNode* firstChild, * secondChild;
			int belowFirst = (ray.m_origin[axis] < currNode->splitPos()) ||
				(ray.m_origin[axis] == currNode->splitPos() && ray.m_dir[axis] <= 0);
			if (belowFirst)
			{
				firstChild = currNode + 1;
				secondChild = &m_nodes[currNode->aboveChild()];
			}
			else
			{
				firstChild = &m_nodes[currNode->aboveChild()];
				secondChild = currNode + 1;
			}

			// Advance to next child node, possibly enqueue other child
			if (tPlane > tMax || tPlane <= 0)
			{
				currNode = firstChild;
			}
			else if (tPlane < tMin)
			{
				currNode = secondChild;
			}
			else
			{
				// Enqueue _secondChild_ in todo list
				todo[todoPos].node = secondChild;
				todo[todoPos].tMin = tPlane;
				todo[todoPos].tMax = tMax;
				++todoPos;
				currNode = firstChild;
				tMax = tPlane;
			}
		}
	}
	return false;
}

bool KdTree::hit(const Ray& ray, SurfaceInteraction& isect) const
{
	// Compute initial parametric range of ray inside kd-tree extent
	Float tMin, tMax;
	if (!m_bounds.hit(ray, tMin, tMax))
	{
		return false;
	}

	// Prepare to traverse kd-tree for ray
	Vector3f invDir(1 / ray.m_dir.x, 1 / ray.m_dir.y, 1 / ray.m_dir.z);
	const int maxTodo = 64;
	KdToDo todo[maxTodo];
	int todoPos = 0;

	// Traverse kd-tree nodes in order for ray
	bool hit = false;
	const KdTreeNode* currNode = &m_nodes[0];
	while (currNode != nullptr)
	{
		// Bail out if we found a hit closer than the current node
		if (ray.m_tMax < tMin)
			break;

		// Process kd-tree interior node
		if (!currNode->isLeaf())
		{
			// Compute parametric distance along ray to split plane
			int axis = currNode->splitAxis();
			Float tPlane = (currNode->splitPos() - ray.m_origin[axis]) * invDir[axis];

			// Get node children pointers for ray
			const KdTreeNode* firstChild, * secondChild;
			int belowFirst = (ray.m_origin[axis] < currNode->splitPos()) ||
				(ray.m_origin[axis] == currNode->splitPos() && ray.m_dir[axis] <= 0);
			if (belowFirst)
			{
				firstChild = currNode + 1;
				secondChild = &m_nodes[currNode->aboveChild()];
			}
			else
			{
				firstChild = &m_nodes[currNode->aboveChild()];
				secondChild = currNode + 1;
			}

			// Advance to next child node, possibly enqueue other child
			if (tPlane > tMax || tPlane <= 0)
			{
				currNode = firstChild;
			}
			else if (tPlane < tMin)
			{
				currNode = secondChild;
			}
			else
			{
				// Enqueue _secondChild_ in todo list
				todo[todoPos].node = secondChild;
				todo[todoPos].tMin = tPlane;
				todo[todoPos].tMax = tMax;
				++todoPos;
				currNode = firstChild;
				tMax = tPlane;
			}
		}
		else
		{
			// Check for intersections inside leaf node
			int nPrimitives = currNode->numPrimitives();
			if (nPrimitives == 1)
			{
				const Primitive::ptr& p = m_Primitives[currNode->m_onePrimitive];
				// Check one Primitive inside leaf node
				if (p->hit(ray, isect))
					hit = true;
			}
			else
			{
				for (int i = 0; i < nPrimitives; ++i)
				{
					int index = m_PrimitiveIndices[currNode->m_PrimitiveIndicesOffset + i];
					const Primitive::ptr& p = m_Primitives[index];
					// Check one Primitive inside leaf node
					if (p->hit(ray, isect))
						hit = true;
				}
			}

			// Grab next node to process from todo list
			if (todoPos > 0)
			{
				currNode = todo[--todoPos].node;
				tMin = todo[todoPos].tMin;
				tMax = todo[todoPos].tMax;
			}
			else
			{
				break;
			}
		}

	}

	return hit;
}

RENDER_END