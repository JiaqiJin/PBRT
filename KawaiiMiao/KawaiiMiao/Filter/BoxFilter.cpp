#include "BoxFilter.h"

RENDER_BEGIN

RENDER_REGISTER_CLASS(BoxFilter, "BoxFilter");

BoxFilter::BoxFilter(const APropertyTreeNode& node)
	: Filter(node.getPropertyList())
{
	activate();
}


Float BoxFilter::evaluate(const Vector2f& p) const
{
	return 1.0f;
}

RENDER_END