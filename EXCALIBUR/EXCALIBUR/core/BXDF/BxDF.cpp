#include "BxDF.h"
#include "../Sampling.h"

RENDERING_BEGIN

Float FrDielectric(Float cosThetaI, Float etaI, Float etaT) {

}

Spectrum FrConductor(Float cosThetaI, const Spectrum& etai,
    const Spectrum& etat, const Spectrum& kt) {

}

Spectrum BxDF::sample_f(const Vector3f& wo,
    Vector3f* wi,
    const Point2f& sample,
    Float* pdf,
    BxDFType* sampledType) const {
    *wi = cosineSampleHemisphere(sample);
    if (wo.z < 0) {
        wi->z *= -1;
    }
    *pdf = pdfDir(wo, *wi);
    return f(wo, *wi);
}

Spectrum BxDF::rho_hd(const Vector3f& wo, int nSamples, const Point2f* samples) const {
    Spectrum ret(0.0f);
    //todo
    return ret;
}

Spectrum BxDF::rho_hh(int nSamples, const Point2f* samplesWo, const Point2f* samplesWi) const {
    Spectrum ret(0.0f);
   
    return ret;
}


Float BxDF::pdfDir(const Vector3f& wo, const Vector3f& wi) const {
    return sameHemisphere(wo, wi) ? absCosTheta(wi) * InvPi : 0;
}

RENDERING_END