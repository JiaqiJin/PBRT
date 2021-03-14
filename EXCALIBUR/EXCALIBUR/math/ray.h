#ifndef ray_h
#define ray_h

#include "../core/header.h"

RENDERING_BEGIN

class Ray {
public:
    Ray() : tMax(Infinity), time(0.f), medium(nullptr) {}

    Ray(const Point3f& ori, const Vector3f& dir, Float tMax = Infinity,
        Float time = 0.f, const Medium* medium = nullptr)
        : ori(ori),
        dir(dir),
        tMax(tMax),
        time(time),
        medium(medium) { }
   
    Point3f at(Float t) const {
        return ori + dir * t;
    }

    bool hasNaNs() const {
        return (ori.hasNaNs() || dir.hasNaNs() || isNaN(tMax));
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

/**
 * 从相机从生成的光线微分，辅助光线的起点与主光线起点相同，
 * 经过高光反射或高光投射之后会渐渐偏离
 */
class RayDifferential : public Ray {
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

    bool hasDifferentials;
    Point3f rxOrigin, ryOrigin;
    Vector3f rxDirection, ryDirection;
};


RENDERING_END

#endif /* ray_h */