#include "MicrofacetReflection.h"

RENDERING_BEGIN

Spectrum MicrofacetReflection::f(const Vector3f& wo, const Vector3f& wi) const {
    Float cosThetaO = AbsCosTheta(wo), cosThetaI = AbsCosTheta(wi);
    Vector3f wh = wi + wo;
    if (cosThetaI == 0 || cosThetaO == 0) return Spectrum(0.);
    if (wh.x == 0 && wh.y == 0 && wh.z == 0) return Spectrum(0.);

    wh = normalize(wh);
    Spectrum F = fresnel->evaluate(dot(wi, wo));
    return R * distribution->D(wh) * distribution->G(wo, wi) * F /
        (4 * cosThetaI * cosThetaO);
}

Spectrum MicrofacetReflection::sample_f(const Vector3f& wo, Vector3f* wi, const Point2f& u,
    Float* pdf, BxDFType* sampledType) const {
    if (wo.z == 0) {
        return 0.;
    }
    Vector3f wh = distribution->sample_wh(wo, u);
    *wi = reflect(wo, wh);
    if (!sameHemisphere(wo, *wi)) {
        return Spectrum(0.f);
    }

    *pdf = distribution->pdfDir(wo, wh) / (4 * dot(wo, wh));
    return f(wo, *wi);
}

Float MicrofacetReflection::pdfDir(const Vector3f& wo, const Vector3f& wi) const {
    if (!sameHemisphere(wo, wi)) {
        return 0;
    }
    Vector3f wh = normalize(wo + wi);
    Float d = dot(wo, wh);
    return distribution->pdfDir(wo, wh) / (4 * d);
}

std::string MicrofacetReflection::toString() const {
    return std::string("[ MicrofacetReflection R: ") + R.ToString() +
        std::string(" distribution: ") + distribution->toString() +
        std::string(" fresnel: ") + fresnel->toString() + std::string(" ]");
}

RENDERING_END