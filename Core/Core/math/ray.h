﻿#ifndef ray_h
#define ray_h

#include "header.h"

KAWAII_BEGIN
 /*
 ray类由一个点作为起点，一个单位向量作为方向
 tMax决定了ray的最远距离
 */

// Ray Declarations
class Ray {
public:
    // Ray Public Methods
    Ray() : tMax(Infinity), time(0.f), medium(nullptr) {

    }

    Ray(const Point3f& ori, const Vector3f& dir, Float tMax = Infinity,
        Float time = 0.f, const Medium* medium = nullptr)
        : ori(ori), dir(dir), tMax(tMax), time(time), medium(medium) {

    }

    Point3f operator()(Float t) const {
        return ori + dir * t;
    }

    bool hasNaNs() const {
        return (ori.hasNaNs() || dir.hasNaNs() || isnan(tMax));
    }

    friend std::ostream& operator<<(std::ostream& os, const Ray& r) {
        os << "[o=" << r.ori << ", d=" << r.dir << ", tMax=" << r.tMax
            << ", time=" << r.time << "]";
        return os;
    }

    // 起点
    Point3f ori;
    // 方向，单位向量
    Vector3f dir;
    // 光线的最远距离
    mutable Float tMax;
    // 发射的时间，用于做motion blur
    Float time;
    // 光线所在的介质(水，空气，玻璃等)
    const Medium* medium;
};

class RayDifferential : public Ray {
public:
    // RayDifferential Public Methods
    RayDifferential() {
        hasDifferentials = false;
    }
    RayDifferential(const Point3f& ori, const Vector3f& dir, Float tMax = Infinity,
        Float time = 0.f, const Medium* medium = nullptr)
        : Ray(ori, dir, tMax, time, medium) {
        hasDifferentials = false;
    }
    RayDifferential(const Ray& ray) : Ray(ray) {
        hasDifferentials = false;
    }
    bool HasNaNs() const {
        return Ray::hasNaNs() ||
            (hasDifferentials &&
                (rxOrigin.hasNaNs() || ryOrigin.hasNaNs() ||
                    rxDirection.hasNaNs() || ryDirection.hasNaNs()));
    }
    void ScaleDifferentials(Float s) {
        rxOrigin = ori + (rxOrigin - ori) * s;
        ryOrigin = ori + (ryOrigin - ori) * s;
        rxDirection = dir + (rxDirection - dir) * s;
        ryDirection = dir + (ryDirection - dir) * s;
    }

    friend std::ostream& operator<<(std::ostream& os, const RayDifferential& r) {
        os << "[ " << (Ray&)r << " has differentials: " <<
            (r.hasDifferentials ? "true" : "false") << ", xo = " << r.rxOrigin <<
            ", xd = " << r.rxDirection << ", yo = " << r.ryOrigin << ", yd = " <<
            r.ryDirection;
        return os;
    }

    // RayDifferential Public Data
    bool hasDifferentials;
    Point3f rxOrigin, ryOrigin;
    Vector3f rxDirection, ryDirection;
};
/*
由于计算出的交点可能会有误差，如果直接把pos作为光线的起点，可能取到的是shape内部的点
如果从内部的点发出光线，则可能产生自相交，为了避免这种情况，通常会对pos做一定的偏移
*/
static Point3f offsetRayOrigin(const Point3f& p, const Vector3f& pError,
    const Normal3f& n, const Vector3f& w) {
    Float d = dot(abs(n), pError);
#ifdef FLOAT_AS_DOUBLE
    d *= 1024.;
#endif
    Vector3f offset = d * Vector3f(n);
    if (dot(w, n) < 0) offset = -offset;
    Point3f po = p + offset;
    // Round offset point _po_ away from _p_
    for (int i = 0; i < 3; ++i) {
        if (offset[i] > 0)
            po[i] = nextFloatUp(po[i]);
        else if (offset[i] < 0)
            po[i] = nextFloatDown(po[i]);
    }
    return po;
}

KAWAII_END

#endif /* ray_h */