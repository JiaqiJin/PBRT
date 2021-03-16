﻿#ifndef disk_hpp
#define disk_hpp

#include "../core/header.h"
#include "../core/Shape.h"

RENDERING_BEGIN

class Disk : public Shape {
public:

    Disk(const Transform* objectToWorld, const Transform* worldToObject,
        bool reverseOrientation, Float height, Float radius, Float innerRadius,
        Float phiMax)
        : Shape(objectToWorld, worldToObject, reverseOrientation),
        _height(height),
        _radius(radius),
        _innerRadius(innerRadius),
        _phiMax(degree2radian(clamp(phiMax, 0, 360))) {

    }

    virtual void init() {
        _invArea = 1 / area();
    }

    virtual AABB3f objectBound() const {
        return AABB3f(Point3f(-_radius, -_radius, _height),
            Point3f(_radius, _radius, _height));
    }

    virtual bool intersect(const Ray& ray, Float* tHit, SurfaceInteraction* isect,
        bool testAlphaTexture) const;

    virtual bool intersectP(const Ray& ray, bool testAlphaTexture) const;

    virtual Float area() const {
        return _phiMax * 0.5 * (_radius * _radius - _innerRadius * _innerRadius);
    }

    virtual Interaction sampleA(const Point2f& u, Float* pdf) const;

private:

    const Float _height;
    const Float _radius;
    const Float _innerRadius;
    const Float _phiMax;
};

RENDERING_END

#endif /* disk_hpp */