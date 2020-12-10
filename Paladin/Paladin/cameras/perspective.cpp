#include "perspective.hpp"
#include "sampling.hpp"

PALADIN_BEGIN

PerspectiveCamera::PerspectiveCamera(const AnimatedTransform& CameraToWorld,
    const AABB2f& screenWindow,
    Float shutterOpen, Float shutterClose,
    Float lensRadius, Float focalDistance,
    Float fov, Film* film,
    const Medium* medium)
    : ProjectiveCamera(CameraToWorld, Transform::perspective(fov, 1e-2f, 1000.f),
        screenWindow, shutterOpen, shutterClose, lensRadius,
        focalDistance, film, medium)
{

}

Float PerspectiveCamera::generateRay(const CameraSample& sample,
    Ray* ray) const
{
    return 1;
}

Float PerspectiveCamera::generateRayDifferential(const CameraSample& sample, RayDifferential* ray) const
{
    return 1;
}

Spectrum PerspectiveCamera::we(const paladin::Ray& ray, Point2f* pRaster2) const {
    // todo 双向方法里用到，暂时不实现
    return Spectrum(0);
}


void PerspectiveCamera::pdfWe(const Ray& ray, Float* pdfPos, Float* pdfDir) const {
    // todo 双向方法里用到，暂时不实现
}

Spectrum PerspectiveCamera::sampleWi(const Interaction& ref, const Point2f& u,
    Vector3f* wi, Float* pdf, Point2f* pRaster,
    VisibilityTester* vis) const {
    // todo 双向方法里用到，暂时不实现
    return Spectrum(0.f);
}

PALADIN_END