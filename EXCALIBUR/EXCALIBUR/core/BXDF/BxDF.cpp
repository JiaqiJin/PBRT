#include "BxDF.h"
#include "../Sampling.h"

RENDERING_BEGIN

Float FrDielectric(Float cosThetaI, Float etaI, Float etaT) {
    cosThetaI = clamp(cosThetaI, -1, 1);
    bool entering = cosThetaI > 0.f;
    if (!entering) {
        std::swap(etaI, etaT);
        cosThetaI = std::abs(cosThetaI);
    }

    Float sinThetaI = std::sqrt(std::max((Float)0, 1 - cosThetaI * cosThetaI));
    Float sinThetaT = etaI / etaT * sinThetaI;
    Float cosThetaT = std::sqrt(std::max((Float)0, 1 - sinThetaT * sinThetaT));

    // 全内部反射情况
    if (sinThetaT >= 1) {
        return 1;
    }

    Float Rparl = ((etaT * cosThetaI) - (etaI * cosThetaT)) /
        ((etaT * cosThetaI) + (etaI * cosThetaT));
    Float Rperp = ((etaI * cosThetaI) - (etaT * cosThetaT)) /
        ((etaI * cosThetaI) + (etaT * cosThetaT));
    return (Rparl * Rparl + Rperp * Rperp) / 2;
}

Spectrum FrConductor(Float cosThetaI, const Spectrum& etai,
    const Spectrum& etat, const Spectrum& kt) {
    cosThetaI = clamp(cosThetaI, -1, 1);
    Spectrum eta = etat / etai;
    Spectrum etak = kt / etai;

    Float cosThetaI2 = cosThetaI * cosThetaI;
    Float sinThetaI2 = 1. - cosThetaI2;
    Spectrum eta2 = eta * eta;
    Spectrum etak2 = etak * etak;

    Spectrum t0 = eta2 - etak2 - sinThetaI2;
    Spectrum a2plusb2 = Sqrt(t0 * t0 + 4 * eta2 * etak2);
    Spectrum t1 = a2plusb2 + cosThetaI2;
    Spectrum a = Sqrt(0.5f * (a2plusb2 + t0));
    Spectrum t2 = (Float)2 * cosThetaI * a;
    Spectrum Rs = (t1 - t2) / (t1 + t2);

    Spectrum t3 = cosThetaI2 * a2plusb2 + sinThetaI2 * sinThetaI2;
    Spectrum t4 = t2 * sinThetaI2;
    Spectrum Rp = Rs * (t3 - t4) / (t3 + t4);

    return 0.5 * (Rp + Rs);
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

// OrenNayar fr(wi,wo) = R/π(A + Bmax(0,cos(φi-φo))sinαtanβ)
Spectrum OrenNayar::f(const Vector3f& wo, const Vector3f& wi) const {
    Float sinThetaI = sinTheta(wi);
    Float sinThetaO = sinTheta(wo);
    // 计算max(0,cos(φi-φo))项
    // 由于三角函数耗时比较高，这里可以用三角恒等变换展开
    // cos(φi-φo) = cosφi * cosφo + sinφi * sinφo
    Float maxCos = 0;
    if (sinThetaI > 1e-4 && sinThetaO > 1e-4) {
        Float sinPhiI = sinPhi(wi), cosPhiI = cosPhi(wi);
        Float sinPhiO = sinPhi(wo), cosPhiO = cosPhi(wo);
        Float dCos = cosPhiI * cosPhiO + sinPhiI * sinPhiO;
        maxCos = std::max((Float)0, dCos);
    }

    Float sinAlpha, tanBeta;
    if (absCosTheta(wi) > absCosTheta(wo)) {
        sinAlpha = sinThetaO;
        tanBeta = sinThetaI / absCosTheta(wi);
    }
    else {
        sinAlpha = sinThetaI;
        tanBeta = sinThetaO / absCosTheta(wo);
    }
    return _R * InvPi * (A + B * maxCos * sinAlpha * tanBeta);
}

std::string OrenNayar::toString() const {
    return std::string("[ OrenNayar R: ") + _R.ToString() +
        StringPrintf(" A: %f B: %f ]", A, B);
}

RENDERING_END