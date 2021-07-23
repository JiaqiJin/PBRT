#include "KDTree.h"
#include "../Tool/Memory.h"

RENDER_BEGIN

class AKdTreeNode
{
public:

private:

};

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