#include "GaussianFilter.h"

RENDER_BEGIN

GaussianFilter::GaussianFilter(const APropertyTreeNode& node)
	: Filter(node.getPropertyList()), alpha(2.0f), //node.getPropertyList().getFloat("alpha")
	expX(std::exp(-alpha * m_radius.x * m_radius.x)),
	expY(std::exp(-alpha * m_radius.y * m_radius.y))
{
	activate();
}

Float GaussianFilter::evaluate(const Vector2f& p) const
{
	return Gaussian(p.x, expX) * Gaussian(p.y, expY);
}

RENDER_END