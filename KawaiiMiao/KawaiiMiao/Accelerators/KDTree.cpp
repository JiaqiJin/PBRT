#include "KDTree.h"
#include "../Tool/Memory.h"

RENDER_BEGIN

// Represent the leaves and interior node of kd-tree.
// Each union member indicate whether a particular field is used for interior nodes, leaf nodes, or both.
class AKdTreeNode
{
public:
	void initLeafNode(int* hitableIndices, int np, std::vector<int>* primitiveIndices);

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
	int numHitables() const { return m_nHitables >> 2; }
	int splitAxis() const { return m_flags & 3; }
	bool isLeaf() const { return (m_flags & 3) == 3; }
	int aboveChild() const { return m_rightChildIndex >> 2; }

	union
	{
		Float m_split; // Split position which is for interior nodes
		int m_oneHitable; // Leaf
		int m_hitableIndicesOffset; // Leaf
	};

private:
	union 
	{
		int m_flags; // Both
		int m_nHitables; // Leaf
		int m_rightChildIndex; // Interior
	};
};

void AKdTreeNode::initLeafNode(int* hitableIndices, int np, std::vector<int>* primitiveIndices)
{
	// Note: the low 2 bits of m_flags which holds the value 3 indicate that it's a leaf node
	//       the upper 30 bits of m_nPrims are available to record how many hitables overlap it.
	m_flags = 3;
	m_nHitables |= (np << 2);

	// Store hitable ids for leaf node
	if (np == 0)
	{
		m_oneHitable = 0;
	}
	else if (np == 1)
	{
		m_oneHitable = hitableIndices[0];
	}
	else
	{
		// Note: if more than one primitive overlaps, then their indices are
		//       stored in a segment of m_primitiveIndicesOffset. The offset
		//       to the first index for the leaf is stored in m_primitiveIndicesOffset
		m_hitableIndicesOffset = primitiveIndices->size();
		for (int i = 0; i < np; ++i)
		{
			primitiveIndices->push_back(hitableIndices[i]);
		}
	}
}

KdTree::KdTree(const std::vector<Hitable::ptr>& hitables, int isectCost/* = 80*/, int traversalCost/* = 1*/,
	Float emptyBonus/* = 0.5*/, int maxHitables/* = 1*/, int maxDepth/* = -1*/) :
	m_isectCost(isectCost),
	m_traversalCost(traversalCost),
	m_maxHitables(maxHitables),
	m_emptyBonus(emptyBonus),
	m_hitables(hitables)
{
	// TODO
}

RENDER_END