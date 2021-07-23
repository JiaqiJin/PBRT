#pragma once

#include "../Core/Rendering.h"
#include "../Math/KMathUtil.h"
#include "../Core/Primitive.h"

RENDER_BEGIN

class KdTreeNode;
class BoundEdge;

class KdTree : public HitableAggregate
{
public:
	typedef std::shared_ptr<KdTree> ptr;

	KdTree(const std::vector<Hitable::ptr>& hitables, int isectCost = 80, int traversalCost = 1,
		Float emptyBonus = 0.5, int maxPrims = 1, int maxDepth = -1);

	virtual std::string toString() const override { return "KdTree[]"; }
private:
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


RENDER_END

