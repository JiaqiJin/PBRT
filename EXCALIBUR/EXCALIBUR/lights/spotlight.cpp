#include "spotlight.h"

RENDERING_BEGIN

SpotLight::SpotLight(const Transform& LightToWorld,
    const MediumInterface& mediumInterface, const Spectrum& I,
    Float totalWidth, Float falloffStart)
    : Light((int)LightFlags::DeltaPosition, LightToWorld, mediumInterface),
    _pos(LightToWorld.exec(Point3f(0, 0, 0))),
    _I(I),
    _cosTotalWidth(std::cos(degree2radian(totalWidth))),
    _cosFalloffStart(std::cos(degree2radian(falloffStart))) {

}

Spectrum SpotLight::sample_Li(const Interaction& ref, const Point2f& u,
    Vector3f* wi, Float* pdf,
    VisibilityTester* vis) const {
    *wi = normalize(_pos - ref.pos);
    *pdf = 1.0f;
    *vis = VisibilityTester(ref, Interaction(_pos, ref.time, mediumInterface));
    return _I * falloff(-*wi) / distanceSquared(_pos, ref.pos);
}


Spectrum SpotLight::sample_Le(const Point2f& u1, const Point2f& u2,
    Float time, Ray* ray, Normal3f* nLight,
    Float* pdfPos, Float* pdfDir) const {
    return _I; //TODO
}

void SpotLight::pdf_Le(const Ray& ray, const Normal3f& nLight,
    Float* pdfPos, Float* pdfDir) const {
  
}

Float SpotLight::falloff(const Vector3f& w) const {
    Vector3f wl = normalize(_worldToLight.exec(w));
    Float cosTheta = wl.z;
    if (cosTheta < _cosTotalWidth) return 0;
    if (cosTheta > _cosFalloffStart) return 1;

    Float delta = (cosTheta - _cosTotalWidth) / (_cosFalloffStart - _cosTotalWidth);
    return (delta * delta) * (delta * delta);
}


Spectrum SpotLight::power() const {
    return _I * _2Pi * (1 - .5f * (_cosFalloffStart + _cosTotalWidth));
}

Float SpotLight::pdf_Li(const Interaction&, const Vector3f&) const {
    return 0.f;
}

RENDERING_END