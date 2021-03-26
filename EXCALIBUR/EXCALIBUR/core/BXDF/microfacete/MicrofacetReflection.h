#pragma once

#include "../BxDF.h"
#include "MicrofacetDistribution.h"

RENDERING_BEGIN

class MicrofacetReflection : public BxDF {
public:
    MicrofacetReflection(const Spectrum& R,
        MicrofacetDistribution* distribution, Fresnel* fresnel)
        : BxDF(BxDFType(BSDF_REFLECTION | BSDF_GLOSSY)),
        R(R),
        distribution(distribution),
        fresnel(fresnel) {
    }

    /*
    *                   D(ωh) G(ωi,ωo) Fr(ωo)
    * fr(p, ωo, ωi) = ------------------------
    *                     4 cosθo cosθi
    */
    Spectrum f(const Vector3f& wo, const Vector3f& wi) const;

    Spectrum sample_f(const Vector3f& wo, Vector3f* wi, const Point2f& u,
        Float* pdf, BxDFType* sampledType) const;

    Float pdfDir(const Vector3f& wo, const Vector3f& wi) const;

    std::string toString() const;

private:
    const Spectrum R;
    const MicrofacetDistribution* distribution;
    const Fresnel* fresnel;
};

RENDERING_END