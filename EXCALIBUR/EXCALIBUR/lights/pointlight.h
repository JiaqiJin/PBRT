﻿#pragma once

#include "../core/light.h"
#include "../core/Shape.h"

RENDERING_BEGIN

class PointLight : public Light {
public:
    PointLight(const Transform& LightToWorld,
        const MediumInterface& mediumInterface, const Spectrum& I)
        :Light((int)LightFlags::DeltaPosition, LightToWorld, mediumInterface),
        _pos(LightToWorld.exec(Point3f(0, 0, 0))),
        _I(I) {

    }

    virtual Spectrum sample_Le(const Point2f& u1, const Point2f& u2,
        Float time, Ray* ray, Normal3f* nLight,
        Float* pdfPos, Float* pdfDir) const override;

    virtual void pdf_Le(const Ray& ray, const Normal3f& nLight,
        Float* pdfPos, Float* pdfDir) const override;

    virtual Spectrum sample_Li(const Interaction& ref, const Point2f& u, Vector3f* wi,
        Float* pdf, VisibilityTester* vis) const override;

    virtual Spectrum power() const override {
        return 4 * Pi * _I;
    }

    virtual Float pdf_Li(const Interaction&, const Vector3f&) const override;

private:
    // 光源位置
    const Point3f _pos;
    // 辐射强度
    const Spectrum _I;
};

RENDERING_END