#include "Filter.h"

RENDER_BEGIN

Float BoxFilter::evaluate(const Vector2f& p) const
{
	return 1.0f;
}

Float GaussianFilter::evaluate(const Vector2f& p) const
{
	return Gaussian(p.x, expX) * Gaussian(p.y, expY);
}

RENDER_END