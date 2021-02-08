#ifndef header_h
#define header_h

// 1字节     uint8_t
// 2字节     uint16_t
// 4字节     uint32_t
// 8字节     uint64_t

#include <iostream>
#include <assert.h>
#include <cmath>
#include <algorithm>
#include <limits>
#include <string>
#include <stdlib.h>
#include <memory>
#include <vector>
#include <atomic>
#include <mutex>
#include <functional>

#ifdef __GNUC__
    //fix 'numeric_limits' is not a member of 'std' for linux
    #include <limits>
    //fix 'memcpy' was not declared in this scope for linux
    #include <string.h>
#endif

#ifdef _MSC_VER
    //fix 二进制“>>”: 没有找到接受“std::string”类型的右操作数
    //的运算符(或没有可接受的转换) for vs2015
    #include <string>
    //fix “max”: 不是“std”的成员 for vs2015
    #include <algorithm>

    #include <memory>
#endif

#include "../tools/stringprint.hpp"
#include "../tools/macro.h"


PALADIN_BEGIN
template<typename T>
class Point2;

template<typename T>
class Point3;

template<typename T>
class Vector3;

template<typename T>
class Vector2;

template<typename T>
class Normal3;

template<typename T>
class AABB3;

template<typename T>
class AABB2;

template<typename T>
class Direction3;

class Medium;

class RGBSpectrum;

class SampledSpectrum;

#ifdef PALADIN_SAMPLED_SPECTRUM
typedef SampledSpectrum Spectrum;
#else
typedef RGBSpectrum Spectrum;
#endif

class Ray;

class Transform;

class AnimatedTransform;

class Shape;

class MediumInteraction;

class MemoryArena;

class Primitive;

class BSDF;

class BxDF;

class VisibilityTester;

class Sampler;

class Scene;

class BSSRDF;

struct Quaternion;

struct Interaction;

struct MediumInterface;

template<typename T, int logBlockSize = 2>
class BlockedArray;

class EFloat;

class Light;

class AreaLight;

class Shape;

class Material;

class Camera;

struct CameraSample;

template <typename T>
class Texture;

class MixMaterial;

PALADIN_END

inline uint32_t floatToBits(float f) {
    uint32_t ui;
    memcpy(&ui, &f, sizeof(float));
    return ui;
}

inline float bitsToFloat(uint32_t ui) {
    float f;
    memcpy(&f, &ui, sizeof(uint32_t));
    return f;
}

inline uint64_t floatToBits(double f) {
    uint64_t ui;
    memcpy(&ui, &f, sizeof(double));
    return ui;
}

inline double bitsToFloat(uint64_t ui) {
    double f;
    memcpy(&f, &ui, sizeof(uint64_t));
    return f;
}

inline float nextFloatUp(float v) {
    if (std::isinf(v) && v > 0.) return v;
    if (v == -0.f) v = 0.f;
    uint32_t ui = floatToBits(v);
    if (v >= 0)
        ++ui;
    else
        --ui;
    return bitsToFloat(ui);
}

inline float nextFloatDown(float v) {
    if (std::isinf(v) && v < 0.) return v;
    if (v == 0.f) v = -0.f;
    uint32_t ui = floatToBits(v);
    if (v > 0)
        --ui;
    else
        ++ui;
    return bitsToFloat(ui);
}

inline double nextFloatUp(double v, int delta = 1) {
    if (std::isinf(v) && v > 0.) return v;
    if (v == -0.f) v = 0.f;
    uint64_t ui = floatToBits(v);
    if (v >= 0.)
        ui += delta;
    else
        ui -= delta;
    return bitsToFloat(ui);
}

inline double nextFloatDown(double v, int delta = 1) {
    if (std::isinf(v) && v < 0.) return v;
    if (v == 0.f) v = -0.f;
    uint64_t ui = floatToBits(v);
    if (v > 0.)
        ui -= delta;
    else
        ui += delta;
    return bitsToFloat(ui);
}

template <typename T>
inline T Mod(T a, T b) {
    T result = a - (a / b) * b;
    return (T)((result < 0) ? result + b : result);
}

template <>
inline Float Mod(Float a, Float b) {
    return std::fmod(a, b);
}

inline Float gammaCorrect(Float value) {
    if (value <= 0.0031308f) return 12.92f * value;
    return 1.055f * std::pow(value, (Float)(1.f / 2.4f)) - 0.055f;
}

inline Float inverseGammaCorrect(Float value) {
    if (value <= 0.04045f) return value * 1.f / 12.92f;
    return std::pow((value + 0.055f) * 1.f / 1.055f, (Float)2.4f);
}

// std的log是以e为底数 log2(x) = lnx / ln2
inline Float Log2(Float x) {
    const Float invLog2 = 1.442695040888963387004650940071;
    return std::log(x) * invLog2;
}

template <typename T>
inline CONSTEXPR bool isPowerOf2(T v) {
    return v && !(v & (v - 1));
}

#define ALLOCA(TYPE, COUNT) (TYPE *) alloca((COUNT) * sizeof(TYPE))


#include "../math/mathutil.h"

template <typename Predicate>
int findInterval(int size, const Predicate& pred) {
    int first = 0, len = size;
    while (len > 0) {
        int half = len >> 1, middle = first + half;
        // Bisect range based on value of _pred_ at _middle_
        if (pred(middle)) {
            first = middle + 1;
            len -= half + 1;
        }
        else
            len = half;
    }
    return paladin::clamp(first - 1, 0, size - 2);
}

inline int32_t roundUpPow2(int32_t v) {
    v--;
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    return v + 1;
}

inline int64_t roundUpPow2(int64_t v) {
    v--;
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    v |= v >> 32;
    return v + 1;
}

inline Float ErfInv(Float x) {
    Float w, p;
    x = paladin::clamp(x, -.99999f, .99999f);
    w = -std::log((1 - x) * (1 + x));
    if (w < 5) {
        w = w - 2.5f;
        p = 2.81022636e-08f;
        p = 3.43273939e-07f + p * w;
        p = -3.5233877e-06f + p * w;
        p = -4.39150654e-06f + p * w;
        p = 0.00021858087f + p * w;
        p = -0.00125372503f + p * w;
        p = -0.00417768164f + p * w;
        p = 0.246640727f + p * w;
        p = 1.50140941f + p * w;
    }
    else {
        w = std::sqrt(w) - 3;
        p = -0.000200214257f;
        p = 0.000100950558f + p * w;
        p = 0.00134934322f + p * w;
        p = -0.00367342844f + p * w;
        p = 0.00573950773f + p * w;
        p = -0.0076224613f + p * w;
        p = 0.00943887047f + p * w;
        p = 1.00167406f + p * w;
        p = 2.83297682f + p * w;
    }
    return p * x;
}

inline Float Erf(Float x) {
    // constants
    Float a1 = 0.254829592f;
    Float a2 = -0.284496736f;
    Float a3 = 1.421413741f;
    Float a4 = -1.453152027f;
    Float a5 = 1.061405429f;
    Float p = 0.3275911f;

    // Save the sign of x
    int sign = 1;
    if (x < 0) sign = -1;
    x = std::abs(x);

    // A&S formula 7.1.26
    Float t = 1 / (1 + p * x);
    Float y =
        1 -
        (((((a5 * t + a4) * t) + a3) * t + a2) * t + a1) * t * std::exp(-x * x);

    return sign * y;
}

#include "../math/vector.h"
#include "../math/point.h"
#include "../math/ray.h"
#include "../math/bounds.h"
#include "../tools/memory.hpp"
#include "../tools/errfloat.h"

PALADIN_BEGIN

inline Float sphericalTheta(const Vector3f& v) {
    return std::acos(clamp(v.z, -1, 1));
}

inline Float sphericalPhi(const Vector3f& v) {
    Float p = std::atan2(v.y, v.x);
    return (p < 0) ? (p + 2 * Pi) : p;
}

inline Vector3f sphericalDirection(Float sinTheta, Float cosTheta, Float phi) {
    return Vector3f(sinTheta * std::cos(phi), sinTheta * std::sin(phi),
        cosTheta);
}

inline Vector3f sphericalDirection(Float sinTheta, Float cosTheta, Float phi,
    const Vector3f& x, const Vector3f& y,
    const Vector3f& z) {
    return sinTheta * std::cos(phi) * x + sinTheta * std::sin(phi) * y + cosTheta * z;
}

PALADIN_END

#endif /* header_h */










