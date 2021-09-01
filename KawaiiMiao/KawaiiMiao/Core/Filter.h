#pragma once

#include "Rendering.h"
#include "Rtti.h"
#include "../Math/KMathUtil.h"

RENDER_BEGIN

class Filter : public AObject
{
public:
	virtual ~Filter() = default;

	Filter(const APropertyList& props);
	Filter(const Vector2f radius)
		: m_radius(radius), m_invRadius(Vector2f(1 / radius.x, 1 / radius.y)) {}

	virtual Float evaluate(const Vector2f& p) const = 0;

	virtual ClassType getClassType() const override { return ClassType::RFilter; }
public:
	const Vector2f m_radius, m_invRadius;
};

RENDER_END