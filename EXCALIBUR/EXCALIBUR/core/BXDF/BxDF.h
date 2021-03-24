#pragma once

#include "../Header.h"
#include "../spectrum.h"
#include "../Material.h"
#include "../sampler.h"
#include "../Interaction.h"

RENDERING_BEGIN

inline Float cosTheta(const Vector3f& w) {
    return w.z;
}

inline Float cos2Theta(const Vector3f& w) {
    return w.z * w.z;
}

inline Float absCosTheta(const Vector3f& w) {
    return std::abs(w.z);
}

inline Float sin2Theta(const Vector3f& w) {
    return std::max((Float)0, (Float)1 - cos2Theta(w));
}

inline Float sinTheta(const Vector3f& w) {
    return std::sqrt(sin2Theta(w));
}

inline Float tanTheta(const Vector3f& w) {
    return sinTheta(w) / cosTheta(w);
}

inline Float tan2Theta(const Vector3f& w) {
    return sin2Theta(w) / cos2Theta(w);
}

inline Float cosPhi(const Vector3f& w) {
    Float _sinTheta = sinTheta(w);
    return (_sinTheta == 0) ? 1 : clamp(w.x / _sinTheta, -1, 1);
}

inline Float sinPhi(const Vector3f& w) {
    Float _sinTheta = sinTheta(w);
    return (_sinTheta == 0) ? 0 : clamp(w.y / _sinTheta, -1, 1);
}

inline Float cos2Phi(const Vector3f& w) {
    return cosPhi(w) * cosPhi(w);
}

inline Float sin2Phi(const Vector3f& w) {
    return sinPhi(w) * sinPhi(w);
}

inline Float cosDPhi(const Vector3f& wa, const Vector3f& wb) {
    return clamp(
        (wa.x * wb.x + wa.y * wb.y) / std::sqrt((wa.x * wa.x + wa.y * wa.y) *
            (wb.x * wb.x + wb.y * wb.y)),
        -1, 1);
}

Float FrDielectric(Float cosThetaI, Float etaI, Float etaT);

Spectrum FrConductor(Float cosThetaI, const Spectrum& etaI,
    const Spectrum& etaT, const Spectrum& kt);

inline Vector3f reflect(const Vector3f& wo, const Vector3f& n) {
    return 2 * dot(wo, n) * n - wo;
}

inline bool refract(const Vector3f& wi, const Normal3f& n, Float eta,
    Vector3f* wt) {

    Float cosThetaI = dot(n, wi);
    Float sin2ThetaI = std::max(Float(0), Float(1 - cosThetaI * cosThetaI));
    Float sin2ThetaT = eta * eta * sin2ThetaI;

    if (sin2ThetaT >= 1) {
        return false;
    }
    Float cosThetaT = std::sqrt(1 - sin2ThetaT);
    *wt = eta * -wi + (eta * cosThetaI - cosThetaT) * Vector3f(n);
    return true;
}

inline bool sameHemisphere(const Vector3f& w, const Vector3f& wp) {
    return w.z * wp.z > 0;
}

inline bool sameHemisphere(const Vector3f& w, const Normal3f& wp) {
    return w.z * wp.z > 0;
}

inline Spectrum schlickFresnel(const Spectrum& R, Float cosTheta) {
    auto pow5 = [](Float a) { return (a * a) * (a * a) * a; };
    return R + (Spectrum(1) - R) * pow5(1 - cosTheta);
}


class BxDF {
public:
    virtual ~BxDF() { }
    BxDF(BxDFType type) : type(type) { }
    bool MatchesFlags(BxDFType t) const {
        return (type & t) == type;
    }

    virtual Spectrum f(const Vector3f& wo, const Vector3f& wi) const = 0;

    virtual Spectrum sample_f(const Vector3f& wo, Vector3f* wi,
        const Point2f& sample, Float* pdf, BxDFType* sampledType = nullptr) const;

    virtual Spectrum rho_hd(const Vector3f& wo, int nSamples,
        const Point2f* samples) const;

    virtual Spectrum rho_hh(int nSamples, const Point2f* samplesWo,
        const Point2f* samplesWi) const;
    virtual Float pdfDir(const Vector3f& wo, const Vector3f& wi) const;

    virtual std::string toString() const = 0;

    virtual Spectrum getColor() const {
        return Spectrum(0.f);
    }

    const BxDFType type;
};

inline std::ostream& operator<<(std::ostream& os, const BxDF& f) {
    os << f.toString();
    return os;
}

class ScaledBxDF : public BxDF {

public:

    ScaledBxDF(BxDF* bxdf, const Spectrum& scale)
        : BxDF(BxDFType(bxdf->type)),
        _bxdf(bxdf),
        _scale(scale) {

    }

    virtual Spectrum rho_hd(const Vector3f& w, int nSamples, const Point2f* samples) const {
        return _scale * _bxdf->rho_hd(w, nSamples, samples);
    }

    virtual Spectrum rho_hh(int nSamples, const Point2f* samples1,
        const Point2f* samples2) const {
        return _scale * _bxdf->rho_hh(nSamples, samples1, samples2);
    }

    virtual Spectrum f(const Vector3f& wo, const Vector3f& wi) const {
        return _scale * _bxdf->f(wo, wi);
    }

    virtual Spectrum sample_f(const Vector3f& wo, Vector3f* wi, const Point2f& sample,
        Float* pdf, BxDFType* sampledType) const {
        Spectrum f = _bxdf->sample_f(wo, wi, sample, pdf, sampledType);
        return _scale * f;
    }

    virtual std::string toString() const {
        return std::string("[ ScaledBxDF bxdf: ") + _bxdf->toString() +
            std::string(" scale: ") + _scale.ToString() + std::string(" ]");
    }

private:
    BxDF* _bxdf;
    Spectrum _scale;
};

// 菲涅尔基类
class Fresnel {
public:
    virtual ~Fresnel() {}

    virtual Spectrum evaluate(Float cosI) const = 0;
    virtual std::string toString() const = 0;
};

inline std::ostream& operator<<(std::ostream& os, const Fresnel& f) {
    os << f.toString();
    return os;
}

class FresnelDielectric : public Fresnel {
public:
    virtual Spectrum evaluate(Float cosThetaI) const {
        return FrDielectric(cosThetaI, _etaI, _etaT);
    }

    FresnelDielectric(Float etaI, Float etaT) : _etaI(etaI), _etaT(etaT) {}

    virtual std::string toString() const {
        return StringPrintf("[ FrenselDielectric etaI: %f etaT: %f ]", _etaI, _etaT);
    }

private:
    Float _etaI, _etaT;
};

class FresnelConductor : public Fresnel {
public:
    virtual Spectrum evaluate(Float cosThetaI) const {
        return FrConductor(std::abs(cosThetaI), _etaI, _etaT, _kt);
    }

    FresnelConductor(const Spectrum& etaI, const Spectrum& etaT,
        const Spectrum& kt)
        : _etaI(etaI), _etaT(etaT), _kt(kt) {

    }

    virtual std::string toString() const {
        return std::string("[ FresnelConductor etaI: ") + _etaI.ToString() +
            std::string(" etaT: ") + _etaT.ToString() + std::string(" k: ") +
            _kt.ToString() + std::string(" ]");
    }

private:
    Spectrum _etaI, _etaT, _kt;
};

class OrenNayar : public BxDF {
public:
    OrenNayar(const Spectrum& R, Float sigma)
        :BxDF(BxDFType(BSDF_REFLECTION | BSDF_DIFFUSE)),
        _R(R) {
        sigma = degree2radian(sigma);
        Float sigma2 = sigma * sigma;
        A = 1.f - (sigma2 / (2.f * (sigma2 + 0.33f)));
        B = 0.45f * sigma2 / (sigma2 + 0.09f);
    }

    // fr(wi,wo) = R/π(A + Bmax(0,cos(φi-φo))sinαtanβ)
    virtual Spectrum f(const Vector3f &wo, const Vector3f &wi) const ;
    
    virtual std::string toString() const;

    FORCEINLINE void setReflection(const Spectrum &R) {
        _R = R;
    }

private:
    Spectrum _R;
    Float A, B;
};

RENDERING_END