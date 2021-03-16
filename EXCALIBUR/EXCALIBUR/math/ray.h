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

constexpr float origin() {
    return 1.f / 32.f;
}

constexpr float floatScale() {
    return 1.f / 65536.f;
}

constexpr float intScale() {
    return 256.f;
}

// ray起点偏移函数，实现参见ray tracing gems chapter 6
inline Point3f offsetRay(const Point3f& p, Normal3f n, const Vector3f& w) {

    n = dot(n, w) > 0 ? n : -n;

    Vector3i of_i(intScale() * n.x, intScale() * n.y, intScale() * n.z);

    Vector3f p_i(
        intToFloat(floatToInt(p.x) + (p.x < 0 ? -of_i.x : of_i.x)),
        intToFloat(floatToInt(p.y) + (p.y < 0 ? -of_i.y : of_i.y)),
        intToFloat(floatToInt(p.z) + (p.z < 0 ? -of_i.z : of_i.z))
    );

    return Point3f(
        fabsf(p.x) < origin() ? p.x + floatScale() * n.x : p_i.x,
        fabsf(p.y) < origin() ? p.y + floatScale() * n.y : p_i.y,
        fabsf(p.z) < origin() ? p.z + floatScale() * n.z : p_i.z
    );
}

inline Point3f offsetRayOrigin(const Point3f& p, const Vector3f& pError,
    const Normal3f& n, const Vector3f& w) {

    Float d = dot(abs(n), pError);
#ifdef FLOAT_AS_DOUBLE
    // 暂时不理解pbrt为何要这样写
    d *= 1024.;
#endif
    Vector3f offset = d * Vector3f(n);

    if (dot(w, n) < 0) {
        // 判断发射方向是向内还是向外
        offset = -offset;
    }

    Point3f po = p + offset;
    // 计算更加保守的值
    for (int i = 0; i < 3; ++i) {
        if (offset[i] > 0) {
            po[i] = nextFloatUp(po[i]);
        }
        else if (offset[i] < 0) {
            po[i] = nextFloatDown(po[i]);
        }
    }
    return po;
}

RENDERING_END

#endif /* ray_h */