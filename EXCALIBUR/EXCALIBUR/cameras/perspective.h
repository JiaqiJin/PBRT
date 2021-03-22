#pragma once

#include "../core/camera.h"

RENDERING_BEGIN

class PerspectiveCamera : public ProjectiveCamera {
public:
    PerspectiveCamera(const AnimatedTransform& CameraToWorld,
        const AABB2f& screenWindow, Float shutterOpen,
        Float shutterClose, Float lensRadius, Float focalDistance,
        Float fov, std::shared_ptr<Film> film, const Medium* medium);

    virtual Float generateRay(const CameraSample& sample, Ray*) const override;

    virtual Float generateRayDifferential(const CameraSample& sample,
        RayDifferential* ray) const override;

    virtual Spectrum We(const Ray& ray, Point2f* pRaster2 = nullptr) const override;

    virtual void pdf_We(const Ray& ray, Float* pdfPos, Float* pdfDir) const override;

    virtual Spectrum sample_Wi(const Interaction& ref, const Point2f& sample,
        Vector3f* wi, Float* pdfDir, Point2f* pRaster,
        VisibilityTester* vis) const override;

private:
    // 向x轴移动一个像素，对应相机空间的变化率
    Vector3f _dxCamera;
    // 向y轴移动一个像素，对应相机空间的变化率
    Vector3f _dyCamera;
    // z=1的情况下的film的面积
    Float _area;
};

RENDERING_END