#pragma once

#include "../core/light.h"

RENDERING_BEGIN

class SpotLight : public Light {
public:
    SpotLight(const Transform& LightToWorld, const MediumInterface& m,
        const Spectrum& I, Float totalWidth, Float falloffStart);

    virtual Spectrum sample_Li(const Interaction& ref, const Point2f& u, Vector3f* wi,
        Float* pdf, VisibilityTester* vis) const override;
    
    Float falloff(const Vector3f& w) const;

    virtual Spectrum sample_Le(const Point2f& u1, const Point2f& u2,
        Float time, Ray* ray, Normal3f* nLight,
        Float* pdfPos, Float* pdfDir) const override;

    virtual void pdf_Le(const Ray& ray, const Normal3f& nLight,
        Float* pdfPos, Float* pdfDir) const override;

    virtual Spectrum power() const override;

    virtual Float pdf_Li(const Interaction&, const Vector3f&) const override;

private:
    const Point3f _pos;
    const Spectrum _I;
    const Float _cosTotalWidth, _cosFalloffStart;
};

RENDERING_END