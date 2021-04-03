#pragma once

#include "../BxDF.h"
#include "MicrofacetDistribution.h"

RENDERING_BEGIN

class MicrofacetTransmission : public BxDF {
public:
    MicrofacetTransmission(const Spectrum& T,
        MicrofacetDistribution* distribution, Float etaA,
        Float etaB, TransportMode mode)
        : BxDF(BxDFType(BSDF_TRANSMISSION | BSDF_GLOSSY)),
        _T(T),
        _distribution(distribution),
        _etaA(etaA),
        _etaB(etaB),
        _fresnel(etaA, etaB),
        _mode(mode) {}

    /*
    * BTDF 函数值
                    η^2 (1 - Fr(ωo)) D(ωh) G(ωi,ωo)   |ωi · ωh||ωo · ωh|
    * fr(ωo, ωi) = ---------------------------------------------------------
    *                   [(ωh · ωo) + η(ωi · ωh)]^2    cosθo cosθi
    */
    virtual Spectrum f(const Vector3f& wo, const Vector3f& wi) const;

    virtual Spectrum sample_f(const Vector3f& wo, Vector3f* wi, const Point2f& u,
        Float* pdf, BxDFType* sampledType) const;

    virtual Float pdfDir(const Vector3f& wo, const Vector3f& wi) const;

    virtual std::string toString() const;

private:
    const Spectrum _T;
    const MicrofacetDistribution* _distribution;
    const Float _etaA, _etaB;
    const FresnelDielectric _fresnel;
    const TransportMode _mode;
};

RENDERING_END