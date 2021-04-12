#pragma once

#include "Header.h"

RENDERING_BEGIN

class PhaseFunction {
public:
    virtual ~PhaseFunction() {}
    virtual Float p(const Vector3f& wo, const Vector3f& wi) const = 0;
    virtual Float sample_p(const Vector3f& wo, Vector3f* wi,
        const Point2f& u) const = 0;

    virtual std::string toString() const = 0;
};

/*
Henyey和Greenstein(1941)开发了一种广泛使用的相位函数。
* 这种相位函数是专门设计的，以方便适合测量散射数据。 measured scattering data
*/
class HenyeyGreenstein : public PhaseFunction {
public:
    HenyeyGreenstein(Float g)
        : _g(g) {}

    virtual Float p(const Vector3f& wo, const Vector3f& wi) const override;

    virtual Float sample_p(const Vector3f& wo, Vector3f* wi,
        const Point2f& u) const override;

    virtual std::string toString() const override {
        return StringPrintf("[ HenyeyGreenstein g: %f ]", _g);
    }
private:
    // 不对称参数 asymmetry parameter
    const Float _g;
};

// Henyey和Greenstein开发的phase函数
inline Float phaseHG(Float cosTheta, Float g) {
    Float denom = 1 + g * g + 2 * g * cosTheta;
    return Inv4Pi * (1 - g * g) / (denom * std::sqrt(denom));
}

// 介质
class Medium {
public:
    // Medium Interface
    virtual ~Medium() {}

    virtual Spectrum Tr(const Ray& ray, Sampler& sampler) const = 0;

    virtual Spectrum sample(const Ray& ray, Sampler& sampler,
        MemoryArena& arena,
        MediumInteraction* mi) const = 0;
};

struct MediumInterface {
    MediumInterface() : inside(nullptr), outside(nullptr) {}

    MediumInterface(const Medium* medium) : inside(medium), outside(medium) {

    }

    MediumInterface(const Medium* inside, const Medium* outside)
        :inside(inside),
        outside(outside) {
    }

    bool isMediumTransition() const {
        return inside != outside;
    }
    // 内部的介质
    const Medium* inside;

    // 外部的介质
    const Medium* outside;
};

RENDERING_END