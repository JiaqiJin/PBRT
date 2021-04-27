#pragma once

#include "Rendering.h"
#include "../Math/KMathUtil.h"

RENDER_BEGIN

class Filter
{
public:
	virtual ~Filter() = default;

	Filter(const Vector2f radius)
		: m_radius(radius), m_invRadius(Vector2f(1 / radius.x, 1 / radius.y)) {}

	virtual Float evaluate(const Vector2f& p) const = 0;

	const Vector2f m_radius, m_invRadius;
};

class BoxFilter final : public Filter
{
public:
	BoxFilter(const Vector2f& radius) : Filter(radius) {}

	virtual Float evaluate(const Vector2f& p) const override;
};

class GaussianFilter final : public Filter
{
public:
	GaussianFilter(const Vector2f& radius, Float alpha)
		: Filter(radius), alpha(alpha),
		expX(std::exp(-alpha * radius.x * radius.x)),
		expY(std::exp(-alpha * radius.y * radius.y)) { }

	Float evaluate(const Vector2f& p) const override;

private:
	Float Gaussian(Float d, Float expv) const 
	{
		return std::max((Float)0, Float(std::exp(-alpha * d * d) - expv));
	}

	const Float alpha; // 2
	const Float expX, expY;
};

RENDER_END