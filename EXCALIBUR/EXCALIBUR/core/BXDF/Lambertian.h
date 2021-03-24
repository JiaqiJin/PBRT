#pragma once

#include "BxDF.h"

RENDERING_BEGIN

class LambertianReflection : public BxDF {
public:
	LambertianReflection(const Spectrum& R)
		: BxDF(BxDFType(BSDF_REFLECTION | BSDF_DIFFUSE)), R(R) { }
    // 朗伯反射中任何方向的反射率都相等
    virtual Spectrum f(const Vector3f& wo, const Vector3f& wi) const override {
        return R * InvPi;
    }

    // 朗伯反射中任何方向的反射率都相等
    virtual Spectrum rho_hd(const Vector3f&, int, const Point2f*) const override {
        return R;
    }

    // 朗伯反射中任何方向的反射率都相等
    virtual Spectrum rho_hh(int, const Point2f*, const Point2f*) const override {
        return R;
    }

    virtual std::string toString() const override {
        return std::string("[ LambertianReflection R: ") + _R.ToString() +
            std::string(" ]");
    }
private:
	const Spectrum R;
};

class LambertianTransmission : public BxDF {
public:
    LambertianTransmission(const Spectrum& T)
        : BxDF(BxDFType(BSDF_TRANSMISSION | BSDF_DIFFUSE)),
        T(T) {

    }

    virtual Spectrum f(const Vector3f& wo, const Vector3f& wi) const {
        return T * InvPi;
    }

    virtual Spectrum rho_hd(const Vector3f&, int, const Point2f*) const {
        return T;
    }

    virtual Spectrum rho_hh(int, const Point2f*, const Point2f*) const {
        return T;
    }

    virtual Spectrum sample_f(const Vector3f& wo, Vector3f* wi, const Point2f& u,
        Float* pdf, BxDFType* sampledType) const;

    virtual Float pdfDir(const Vector3f& wo, const Vector3f& wi) const;

    virtual std::string toString() const;

private:
    // 透射系数
    Spectrum T;
};

RENDERING_END