#include "gaussian.h"

RENDERING_BEGIN

Float GaussianFilter::Evaluate(const Point2f& p) const {
	return Gaussian(p.x, expX) * Gaussian(p.y, expY);
}

RENDERING_END