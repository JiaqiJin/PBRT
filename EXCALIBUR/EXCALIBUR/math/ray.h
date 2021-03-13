#ifndef ray_h
#define ray_h

#include "../core/Header.h"

RENDERING_BEGIN

class Ray
{
public:
    Ray() : tMax(Infinity),
        time(0.f),
        medium(nullptr),
        tMin(RayEpsilon) {}

    Ray(const Point3f& ori, const Vector3f& dir, Float tMax = Infinity,
        Float time = 0.f, const Medium* medium = nullptr,
        Float tMin = RayEpsilon)
        : ori(ori),
        dir(dir),
        tMax(tMax),
        time(time),
        medium(medium),
        tMin(tMin) {}

    Point3f at(Float t) const {
        return ori + dir * t;
    }

    bool isValid() const {
        return !dir.isZero();
    }

    bool hasNaNs() const {
        return (ori.hasNaNs() || dir.hasNaNs() || isNaN(tMax));
    }

    friend std::ostream& operator<<(std::ostream& os, const Ray& r) {
        os << "[o=" << r.ori << ", d=" << r.dir << ", tMax=" << r.tMax
            << ", time=" << r.time << "]";
        return os;
    }

public:
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

    mutable Float tMin;
};

//从相机从生成的光线微分，辅助光线的起点与主光线起点相同，经过高光反射或高光投射之后会渐渐偏离
class RayDifferential : Ray
{
public:
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

    void scaleDifferentials(Float s) {
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

public:
    bool hasDifferentials;
    // x,y方向微分光线的起点
    Point3f rxOrigin, ryOrigin;
    // x,y方向微分光线的方向
    Vector3f rxDirection, ryDirection;
};

constexpr float origin() {
    return 1.f / 32.f;
}

constexpr float floatScale() {
    return 1.f / 65536.f;
}

constexpr float intScale() {
    return 256.f;
}

RENDERING_END

#endif /* ray_h */