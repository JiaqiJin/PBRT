#pragma once

#include "transform.h"
#include "quaternion.h"

RENDERING_BEGIN

struct Interval {
    Interval(Float v) : low(v), high(v) {}
    Interval(Float v0, Float v1)
        : low(std::min(v0, v1)), high(std::max(v0, v1)) {}

    Interval operator + (const Interval& i) const {
        return Interval(low + i.low, high + i.high);
    }

    Interval operator - (const Interval& i) const {
        return Interval(low - i.high, high - i.low);
    }

    Interval operator * (const Interval& i) const {
        Float ll = low * i.low;
        Float hl = high * i.low;
        Float hh = high * i.high;
        Float lh = low * i.high;
        return Interval(std::min(std::min(ll, hl),
            std::min(lh, hh)),
            std::max(std::max(ll, hl),
                std::max(lh, hh)));
    }

    Float low, high;
};

inline Interval sin(const Interval& i) {
    CHECK_GE(i.low, 0);
    CHECK_LE(i.high, 2.0001 * Pi);
    Float sinLow = std::sin(i.low);
    Float sinHigh = std::sin(i.high);
    if (sinLow > sinHigh) {
        std::swap(sinLow, sinHigh);
    }
    if (i.low < Pi / 2 && i.high > Pi / 2) {
        sinHigh = 1.;
    }
    if (i.low < (3.f / 2.f) * Pi && i.high >(3.f / 2.f) * Pi) {
        sinLow = -1.;
    }
    return Interval(sinLow, sinHigh);
}

inline Interval cos(const Interval& i) {
    CHECK_GE(i.low, 0);
    CHECK_LE(i.high, 2.0001 * Pi);
    Float cosLow = std::cos(i.low);
    Float cosHigh = std::cos(i.high);
    if (cosLow > cosHigh) {
        std::swap(cosLow, cosHigh);
    }
    if (i.low < Pi && i.high > Pi) {
        cosLow = -1.;
    }
    return Interval(cosLow, cosHigh);
}

inline void intervalFindZeros(Float c1, Float c2, Float c3, Float c4, Float c5,
    Float theta, Interval tInterval, Float* zeros,
    int* zeroCount, int depth = 8) {
    // 先根据t的取值范围(函数定义域)求出f'(t)的范围(函数值域)range
    Interval range = Interval(c1) +
        (Interval(c2) + Interval(c3) * tInterval) *
        cos(Interval(2 * theta) * tInterval) +
        (Interval(c4) + Interval(c5) * tInterval) *
        sin(Interval(2 * theta) * tInterval);
    // 如果f'(t)最大值小于零说明单调递减
    // 如果f'(t)最大值大于零说明单调递增
    // 如果f'(t)最大值等于最小值，f(t)值不变
    // 以上三种情况都可以把区间端点当做最值点
    if (range.low > 0. || range.high < 0. || range.low == range.high)
        return;
    if (depth > 0) {
        // 把区间平均分成两段，求极值点
        Float mid = (tInterval.low + tInterval.high) * 0.5f;
        intervalFindZeros(c1, c2, c3, c4, c5, theta,
            Interval(tInterval.low, mid), zeros, zeroCount,
            depth - 1);
        intervalFindZeros(c1, c2, c3, c4, c5, theta,
            Interval(mid, tInterval.high), zeros, zeroCount,
            depth - 1);
    }
    else {
        Float t = (tInterval.low + tInterval.high) * 0.5f;
        for (int i = 0; i < 4; ++i) {

            Float dfdt = c1 + (c2 + c3 * t) * std::cos(2.f * theta * t)
                + (c4 + c5 * t) * std::sin(2.f * theta * t);
            Float d2fdt2 = (c3 + 2 * (c4 + c5 * t) * theta) * std::cos(2.f * t * theta)
                + (c5 - 2 * (c2 + c3 * t) * theta) * std::sin(2.f * t * theta);
            if (dfdt == 0 || d2fdt2 == 0)
                break;
            t = t - dfdt / d2fdt2;
        }
        if (t >= tInterval.low - 1e-3f &&
            t < tInterval.high + 1e-3f) {
            // 如果在该区间内找到解，则记录，否则不记录
            zeros[*zeroCount] = t;
            (*zeroCount)++;
        }
    }
}

class AnimatedTransform {

public:
   
    AnimatedTransform(const Transform* startTransform, Float startTime,
        const Transform* endTransform, Float endTime);

    static void decompose(const Matrix4x4& m, Vector3f* T, Quaternion* R,
        Matrix4x4* S);

    void interpolate(Float time, Transform* t) const;

    Transform interpolate(Float time) const;

    Ray exec(const Ray& r) const;

    RayDifferential exec(const RayDifferential& r) const;

    /*
     获取一个包围盒对象，返回运动过程中包围盒扫过的范围的包围盒
     */
    AABB3f motionAABB(const AABB3f& b) const;

    /*
     获取点对象，返回运动过程中点扫过的范围的包围盒
     */
    AABB3f boundPointMotion(const Point3f& p) const;

private:

    const Transform* _startTransform;
    const Transform* _endTransform;
    const Float _startTime;
    const Float _endTime;
    const bool _actuallyAnimated;

    bool _hasRotation;
    Vector3f _T[2];
    Quaternion _R[2];
    Matrix4x4 _S[2];

    struct DerivativeTerm {
        DerivativeTerm() {}
        DerivativeTerm(Float c, Float x, Float y, Float z)
            : kc(c), kx(x), ky(y), kz(z) {}
        Float kc, kx, ky, kz;
        Float Eval(const Point3f& p) const {
            return kc + kx * p.x + ky * p.y + kz * p.z;
        }
    };
    // xyz三个维度，所以要用数组
    DerivativeTerm c1[3], c2[3], c3[3], c4[3], c5[3];
};

RENDERING_END