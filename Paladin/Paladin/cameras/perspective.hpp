﻿//
//  perspective.hpp
//  Paladin
//
#ifndef perspective_hpp
#define perspective_hpp

#include "header.h"
#include "camera.hpp"

PALADIN_BEGIN

class PerspectiveCamera : public ProjectiveCamera
{
public:
    PerspectiveCamera(const AnimatedTransform& CameraToWorld,
        const AABB2f& screenWindow, Float shutterOpen,
        Float shutterClose, Float lensRadius, Float focalDistance,
        Float fov, Film* film, const Medium* medium);

    virtual Float generateRay(const CameraSample& sample, Ray*) const;

    virtual Float generateRayDifferential(const CameraSample& sample,
        RayDifferential* ray) const;

    virtual Spectrum we(const Ray& ray, Point2f* pRaster2 = nullptr) const;

    virtual void pdfWe(const Ray& ray, Float* pdfPos, Float* pdfDir) const;

    virtual Spectrum sampleWi(const Interaction& ref, const Point2f& sample,
        Vector3f* wi, Float* pdf, Point2f* pRaster,
        VisibilityTester* vis) const;

private:
    // 向x轴移动一个像素，对应相机空间的变化率
    Vector3f _dxCamera;
    // 向y轴移动一个像素，对应相机空间的变化率
    Vector3f _dyCamera;
    // z=1的情况下的film的面积
    Float _area;
};

PALADIN_END

#endif /* perspective_hpp */
