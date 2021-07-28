#pragma once

#include "../Core/Rendering.h"
#include "../Math/KMathUtil.h"
#include "../Core/Primitive.h"

RENDER_BEGIN

class KdTreeNode;
class BoundEdge;

// The KD-Tree is a binary tree, where each interior node always has both children and 
// where leaves of the tree store the primitive  that overlap them.
class KdTree : public HitableAggregate
{
public:
	typedef std::shared_ptr<KdTree> ptr;

	KdTree(const std::vector<Hitable::ptr>& hitables, int isectCost = 80, int traversalCost = 1,
		Float emptyBonus = 0.5, int maxPrims = 1, int maxDepth = -1);

	virtual std::string toString() const override { return "KdTree[]"; }

	virtual bool hit(const Ray& ray) const override;
	virtual bool hit(const Ray& ray, SurfaceInteraction& iset) const override;

private:
	void buildTree(int nodeNum, const Bounds3f& bounds,
		const std::vector<Bounds3f>& primBounds, int* primNums,
		int nprims, int depth,
		const std::unique_ptr<BoundEdge[]> edges[3], int* prims0,
		int* prims1, int badRefines = 0);

	// SAH split measurement
	const Float m_emptyBonus;
	const int m_isectCost, m_traversalCost, m_maxHitables;

	Bounds3f m_bounds;
	std::vector<Hitable::ptr> m_hitables;
	std::vector<int> m_hitableIndices;

	// Compact node into an array
	KdTreeNode* m_nodes;
	int m_nAllocedNodes, m_nextFreeNode;
};

struct KdToDo
{
	const KdTreeNode* node;
	Float tMin, tMax;
};

RENDER_END

