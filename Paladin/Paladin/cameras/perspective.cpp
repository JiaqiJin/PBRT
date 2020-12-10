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
    // 通过光栅空间的微分获取到相机空间的微分 Compute differential changes in origin for perspective camera rays
    _dxCamera = (_rasterToCamera.exec(Point3f(1, 0, 0)) - _rasterToCamera.exec(Point3f(0, 0, 0)));
    _dyCamera = (_rasterToCamera.exec(Point3f(0, 1, 0)) - _rasterToCamera.exec(Point3f(0, 0, 0)));

    //计算相机空间下，z = 1的情况下的film面积 Compute image plane bounds at z=1 for PerspectiveCamera
    Point2f res = film->fullResolution;
    Point3f pMin = _rasterToCamera.exec(Point3f(0, 0, 0));
    Point3f pMax = _rasterToCamera.exec(Point3f(res.x, res.y, 0));
    pMin /= pMin.z;
    pMax /= pMax.z;
    _area = std::abs((pMax.x - pMin.x) * (pMax.y - pMin.y));
}

Float PerspectiveCamera::generateRay(const CameraSample& sample,
    Ray* ray) const
{
    // 在胶片上采样的点 Compute raster and camera sample positions
    Point3f pFilm = Point3f(sample.pFilm.x, sample.pFilm.y, 0);
    // 把光栅空间转到相机空间
    Point3f pCamera = _rasterToCamera.exec(pFilm);
    // 生成光线  ori                  dir
    *ray = Ray(Point3f(0,0,0), normalize(Vector3f(pCamera)));

    if (_lensRadius > 0) {
        Point2f pLens = _lensRadius * uniformSampleDisk(sample.pLens);

        // 计算焦平面
        Float ft = _focalDistance / ray->dir.z;
        Point3f pFocus = ray->at(ft);

        // 由于透镜的影响，更新ray属性
        ray->ori = Point3f(pLens.x, pLens.y, 0);
        ray->dir = normalize(pFocus - ray->ori);
    }
    ray->time = lerp(sample.time, shutterOpen, shutterClose);
    ray->medium = medium;
    *ray = cameraToWorld.exec(*ray);
    return 1;
}

Float PerspectiveCamera::generateRayDifferential(const CameraSample& sample, RayDifferential* ray) const
{
    //A ray’s direction = CameraSample pFilm location
    Point3f pFilm = Point3f(sample.pFilm.x, sample.pFilm.y, 0);
    Point3f pCamera = _rasterToCamera.exec(pFilm);
    //ray’s vector direction is component-wise equal to this point’s position
    Vector3f dir = normalize(Vector3f(pCamera));
    *ray = RayDifferential(Point3f(0, 0, 0), dir);

    if (_lensRadius > 0) {
        Point2f pLens = _lensRadius * uniformSampleDisk(sample.pLens);

        // 计算焦平面
        Float ft = _focalDistance / ray->dir.z;
        Point3f pFocus = ray->at(ft);
        // 由于透镜的影响，更新ray属性
        ray->ori = Point3f(pLens.x, pLens.y, 0);
        ray->dir = normalize(pFocus - ray->ori);
    }
    // 计算光线微分
    if (_lensRadius > 0) {
        Point2f pLens = _lensRadius * uniformSampleDisk(sample.pLens);
        Vector3f dx = normalize(Vector3f(pCamera + _dxCamera));
        Float ft = _focalDistance / dx.z;
        Point3f pFocus = Point3f(0, 0, 0) + (ft * dx);
        ray->rxOrigin = Point3f(pLens.x, pLens.y, 0);
        ray->rxDirection = normalize(pFocus - ray->rxOrigin);

        Vector3f dy = normalize(Vector3f(pCamera + _dyCamera));
        ft = _focalDistance / dy.z;
        pFocus = Point3f(0, 0, 0) + (ft * dy);
        ray->ryOrigin = Point3f(pLens.x, pLens.y, 0);
        ray->ryDirection = normalize(pFocus - ray->ryOrigin);
    }
    else{
        ray->rxOrigin = ray->ryOrigin = ray->ori;
        ray->rxDirection = normalize(Vector3f(pCamera) + _dxCamera);
        ray->ryDirection = normalize(Vector3f(pCamera) + _dyCamera);
    }
    ray->time = lerp(sample.time, shutterOpen, shutterClose);
    ray->medium = medium;
    *ray = cameraToWorld.exec(*ray);
    ray->hasDifferentials = true;
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