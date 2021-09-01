#pragma once

#include "../Core/Filter.h"

RENDER_BEGIN

class GaussianFilter final : public Filter
{
public:
	GaussianFilter(const APropertyTreeNode& node);
	GaussianFilter(const Vector2f& radius, Float alpha)
		: Filter(radius), alpha(alpha),
		expX(std::exp(-alpha * radius.x * radius.x)),
		expY(std::exp(-alpha * radius.y * radius.y)) { }

	Float evaluate(const Vector2f& p) const override;

	virtual std::string toString() const override { return "GaussianFilter[]"; }
private:
	Float Gaussian(Float d, Float expv) const
	{
		return std::max((Float)0, Float(std::exp(-alpha * d * d) - expv));
	}

	const Float alpha; // 2
	const Float expX, expY;
};

RENDER_END