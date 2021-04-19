#pragma once

#include "Header.h"
#include "../math/rng.h"

RENDERING_BEGIN

static __forceinline Point2f uniformSampleSector(const Point2f& u, Float thetaMax) {
    Float r = std::sqrt(u[0]);
    Float theta = thetaMax * u[1];
    return Point2f(r * std::cos(theta), r * std::sin(theta));
}

static __forceinline Point2f uniformSampleDisk(const Point2f& u) {
    Float r = std::sqrt(u[0]);
    Float theta = _2Pi * u[1];
    return Point2f(r * std::cos(theta), r * std::sin(theta));
}

static __forceinline Point2f concentricSampleDisk(const Point2f& u) {
    // 把[0,1]映射到[-1,1]
    Point2f uOffset = 2.f * u - Vector2f(1, 1);

    // 退化到原点的情况
    if (uOffset.x == 0 && uOffset.y == 0) {
        return Point2f(0, 0);
    }

    // r = x
    // θ = y / x * π / 4
    Float theta, r;
    if (std::abs(uOffset.x) > std::abs(uOffset.y)) {
        r = uOffset.x;
        theta = PiOver4 * (uOffset.y / uOffset.x);
    }
    else {
        r = uOffset.y;
        theta = PiOver2 - PiOver4 * (uOffset.x / uOffset.y);
    }
    return r * Point2f(std::cos(theta), std::sin(theta));
}

static __forceinline Vector3f uniformSampleSphere(const Point2f& u) {
    Float z = 1 - 2 * u[0];
    Float r = std::sqrt(std::max((Float)0, (Float)1 - z * z));
    Float phi = _2Pi * u[1];
    return Vector3f(r * std::cos(phi), r * std::sin(phi), z);
}

static __forceinline Float uniformConePdf(Float cosThetaMax) {
    return 1 / (_2Pi * (1 - cosThetaMax));
}

static __forceinline Vector3f UniformSampleCone(const Point2f& u, Float cosThetaMax) {
    Float cosTheta = ((Float)1 - u[0]) + u[0] * cosThetaMax;
    Float sinTheta = std::sqrt((Float)1 - cosTheta * cosTheta);
    Float phi = u[1] * _2Pi;
    return Vector3f(std::cos(phi) * sinTheta, std::sin(phi) * sinTheta,
        cosTheta);
}

static __forceinline Point2f uniformSampleTriangle(const Point2f& u) {
    Float su0 = std::sqrt(u[0]);
    return Point2f(1 - su0, u[1] * su0);
}

inline Vector3f cosineSampleHemisphere(const Point2f& u) {
    Point2f d = uniformSampleDisk(u);
    Float z = std::sqrt(std::max((Float)0, 1 - d.x * d.x - d.y * d.y));
    return Vector3f(d.x, d.y, z);
}

template <typename T>
void shuffle(T* samp, int count, int nDimensions, RNG& rng) {
    for (int i = 0; i < count; ++i) {
        // 随机选择一个i右侧的索引
        int other = i + rng.uniformUInt32(count - i);
        for (int j = 0; j < nDimensions; ++j) {
            std::swap(samp[nDimensions * i + j], samp[nDimensions * other + j]);
        }
    }
}

struct Distribution1D {

    Distribution1D() {}
    
    Distribution1D(const Float* f, int num)
    : _func(f, f + num), 
      _cdf(num + 1) {
        _cdf[0] = 0;
        for (int i = 1; i < num + 1; ++i) {
            _cdf[i] = _cdf[i - 1] + _func[i - 1] / num;
        }

        // 由于func积分值不一定为1
        // 所以需要将_cdf归一化
        _funcInt = _cdf[num];
        if (_funcInt == 0) {
            // 如果_func全为零，则均匀分布
            for (int i = 1; i < num + 1; ++i) {
                _cdf[i] = Float(i) / Float(num);
            }
        }
        else {
            for (int i = 1; i < num + 1; ++i) {
                _cdf[i] = _cdf[i] / _funcInt;
            }
        }
    }

    int count() const {
        return (int)_func.size();
    }

private:
    // 指定分布的函数
    std::vector<Float> _func;
    // 指定函数的累积分布函数
    std::vector<Float> _cdf;
    // func函数的积分值
    Float _funcInt;

    friend struct Distribution2D;
};

struct Distribution2D {

};

RENDERING_END