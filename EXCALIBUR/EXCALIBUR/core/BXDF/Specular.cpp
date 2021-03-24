#include "Specular.h"

RENDERING_BEGIN

Spectrum SpecularReflection::sample_f(const Vector3f& wo, Vector3f* wi, const Point2f& sample,
    Float* pdf, BxDFType* sampledType) const {
    *wi = Vector3f(-wo.x, -wo.y, wo.z);
    *pdf = 1;
    return fresnel->evaluate(cosTheta(*wi)) * R / absCosTheta(*wi);
}

std::string SpecularReflection::toString() const {
    return std::string("[ SpecularReflection R: ") + R.ToString() +
        std::string(" fresnel: ") + fresnel->toString() + std::string(" ]");
}

// SpecularTransmission
Spectrum SpecularTransmission::sample_f(const Vector3f& wo, Vector3f* wi, const Point2f& sample,
    Float* pdf, BxDFType* sampledType) const {
    // 首先确定光线是进入或离开折射介质
    // 对象的法线都是向外的
    // 如果wo.z > 0，则说明，ray trace工作流的光线从物体外部射入物体
    bool entering = cosTheta(wo) > 0;
    Float etaI = entering ? etaA : etaB;
    Float etaT = entering ? etaB : etaA;

    if (!refract(wo, faceforward(Normal3f(0, 0, 1), wo), etaI / etaT, wi)) {
        return 0;
    }

    *pdf = 1;
    Spectrum ft = T * (Spectrum(1.) - fresnel.evaluate(cosTheta(*wi)));

    // 用于处理双向方法的情况，只有从光源射出的光线才需要乘以这个缩放因子
    if (mode == TransportMode::Radiance) {
        ft *= (etaI * etaI) / (etaT * etaT);
    }
    return ft / absCosTheta(*wi);
}

std::string SpecularTransmission::toString() const {
    return std::string("[ SpecularTransmission: T: ") + T.ToString() +
        StringPrintf(" etaA: %f etaB: %f ", etaA, etaB) +
        std::string(" fresnel: ") + fresnel.toString() +
        std::string(" mode : ") +
        (mode == TransportMode::Radiance ? std::string("RADIANCE")
            : std::string("IMPORTANCE")) +
        std::string(" ]");
}

RENDERING_END