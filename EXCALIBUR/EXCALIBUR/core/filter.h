#pragma once

#include "Header.h"

RENDERING_BEGIN

// Filter Declarations
class Filter {
public:
    // Filter Interface
    virtual ~Filter();
    Filter(const Vector2f& radius)
        : radius(radius), invRadius(Vector2f(1 / radius.x, 1 / radius.y)) {}
    virtual Float Evaluate(const Point2f& p) const = 0;

    // Filter Public Data
    const Vector2f radius, invRadius;
};


RENDERING_END