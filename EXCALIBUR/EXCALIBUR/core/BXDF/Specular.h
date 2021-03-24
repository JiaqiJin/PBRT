#pragma once

#include "BxDF.h"

RENDERING_BEGIN

// 理想镜面反射 狄拉克delta分布
class SpecularReflection : public BxDF {
public:
    SpecularReflection(const Spectrum& R, Fresnel* fresnel)
        : BxDF(BxDFType(BSDF_REFLECTION | BSDF_SPECULAR)), R(R),
        fresnel(fresnel) { }

    virtual Spectrum f(const Vector3f& wo, const Vector3f& wi) const {
        return Spectrum(0.f);
    }

    virtual Spectrum sample_f(const Vector3f& wo, Vector3f* wi, const Point2f& sample,
        Float* pdf, BxDFType* sampledType) const;

    virtual Float pdfDir(const Vector3f& wo, const Vector3f& wi) const {
        return 0;
    }

    virtual std::string toString() const;

private:
    const Spectrum R;
    const Fresnel* fresnel;
};

// 理想镜面透射， 当光线照射在水面（或者其他介质上）， 一部分光会被反射（反射光可以直接用菲涅尔函数计）
// 折射光的radiance就没那么简单，因为在两个介质交界处，光线的微分立体角会发生变化(因为折射率不同)
// 根据能量守恒定律，微分立体角发生变化，radiance也就发生了变化, 我们用τ表示被折射的能量的比率则τ = 1 - Fr(wi)
class SpecularTransmission : public BxDF {
public:
    SpecularTransmission(const Spectrum& T, Float etaA, Float etaB,
        TransportMode mode)
        : BxDF(BxDFType(BSDF_TRANSMISSION | BSDF_SPECULAR)), T(T), etaA(etaA),
        etaB(etaB), fresnel(etaA, etaB), mode(mode) {
    }

    virtual Spectrum f(const Vector3f& wo, const Vector3f& wi) const {
        return Spectrum(0.f);
    }

    virtual Spectrum sample_f(const Vector3f& wo, Vector3f* wi, const Point2f& sample,
        Float* pdf, BxDFType* sampledType) const;

    virtual Float pdfDir(const Vector3f& wo, const Vector3f& wi) const {
        return 0;
    }

    virtual std::string toString() const;

private:
    // 用于缩放颜色频谱
    const Spectrum T;
    // etaA是表面以上介质的折射率 (above)
    // etaB是物体表面以下介质的折射率 (below)
    const Float etaA, etaB;
    const FresnelDielectric fresnel;
    const TransportMode mode;
};

RENDERING_END