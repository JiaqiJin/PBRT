#include "pointlight.h"
#include "../core/Sampling.h"

RENDERING_BEGIN

Spectrum PointLight::sample_Li(const Interaction& ref, const Point2f& u,
    Vector3f* wi, Float* pdf,
    VisibilityTester* vis) const {
    *wi = normalize(_pos - ref.pos);
    *pdf = 1.0f;
    *vis = VisibilityTester(ref, Interaction(_pos, ref.time, mediumInterface));
    return _I / distanceSquared(_pos, ref.pos);
}

Float PointLight::pdf_Li(const Interaction&, const Vector3f&) const {
    return 0;
}

Spectrum PointLight::sample_Le(const Point2f& u1, const Point2f& u2,
    Float time, Ray* ray, Normal3f* nLight,
    Float* pdfPos, Float* pdfDir) const {
    return _I; //TODO
}

void PointLight::pdf_Le(const Ray& ray, const Normal3f& nLight,
    Float* pdfPos, Float* pdfDir) const {
    //TODO
}

RENDERING_END