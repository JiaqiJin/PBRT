#pragma once

#include "../Core/Rendering.h"
#include "../Math/KMathUtil.h"
#include "../Core/Primitive.h"

RENDER_BEGIN

class KdTreeNode;
class BoundEdge;

class KdTree : public PrimitiveAggregate
{
public:
	typedef std::shared_ptr<KdTree> ptr;

	KdTree(const std::vector<Primitive::ptr>& Primitives, int isectCost = 80, int traversalCost = 1,
		Float emptyBonus = 0.5, int maxPrims = 1, int maxDepth = -1);

	virtual Bounds3f worldBound() const override { return m_bounds; }
	~KdTree();

	virtual bool hit(const Ray& ray) const override;
	virtual bool hit(const Ray& ray, SurfaceInteraction& iset) const override;

	virtual std::string toString() const override { return "KdTree[]"; }

private:

	void buildTree(int nodeNum, const Bounds3f& bounds,
		const std::vector<Bounds3f>& primBounds, int* primNums,
		int nprims, int depth,
		const std::unique_ptr<BoundEdge[]> edges[3], int* prims0,
		int* prims1, int badRefines = 0);

	// SAH split measurement
	const Float m_emptyBonus;
	const int m_isectCost, m_traversalCost, m_maxPrimitives;

	// Compact the node into an array
	KdTreeNode* m_nodes;
	int m_nAllocedNodes, m_nextFreeNode;

	Bounds3f m_bounds;
	std::vector<Primitive::ptr> m_Primitives;
	std::vector<int> m_PrimitiveIndices;
};

struct KdToDo
{
	const KdTreeNode* node;
	Float tMin, tMax;
};

RENDER_END

