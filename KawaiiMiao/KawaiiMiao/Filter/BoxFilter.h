#pragma once

#include "../Core/Filter.h"

RENDER_BEGIN

class BoxFilter final : public Filter
{
public:
	BoxFilter(const APropertyTreeNode& node);
	BoxFilter(const Vector2f& radius) : Filter(radius) {}

	virtual Float evaluate(const Vector2f& p) const override;

	virtual std::string toString() const override { return "BoxFilter[]"; }
};

RENDER_END