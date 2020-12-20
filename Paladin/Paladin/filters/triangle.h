#ifndef filters_triangle_h
#define filters_triangle_h

#include "filter.h"

PALADIN_BEGIN

class TriangleFilter : public Filter
{
public:
    TriangleFilter(const Vector2f& radius) : Filter(radius) {
    }

    virtual Float evaluate(const Point2f& p) const {
        return std::max((Float)0, radius.x - std::abs(p.x)) *
            std::max((Float)0, radius.y - std::abs(p.y));
    }
};

PALADIN_END

#endif 
