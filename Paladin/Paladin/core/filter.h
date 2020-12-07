#ifndef filter_h
#define filter_h

#include "header.h"

PALADIN_BEGIN

class Filter
{
public:
    virtual ~Filter() {}

    Filter(const Vector2f& radius)
        : radius(radius),
        invRadius(Vector2f(1 / radius.x, 1 / radius.y)) {

    }
    virtual Float evaluate(const Point2f& p) const = 0;

    const Vector2f radius;
    const Vector2f invRadius;
};

PALADIN_END

#endif