#pragma once

#include "../core/filter.h"

RENDERING_BEGIN

class GaussianFilter : public Filter {
public:
	GaussianFilter(const Vector2f r, Float alpha)
		:Filter(r),
		_alpha(alpha),
		expX(std::exp(-_alpha * r.x * -r.x)),
		expY(std::exp(-_alpha * r.y * -r.y)) {}

	Float Evaluate(const Point2f& p) const;

private:
	const Float _alpha;
	const Float expX, expY;

	Float Gaussian(Float d, Float expv) const {
		return std::max((Float)0, Float(std::exp(-_alpha * d * d) - expv));
	}
};

RENDERING_END