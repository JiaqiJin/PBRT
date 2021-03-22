#pragma once

#include "Header.h"
#include "film.h"
#include "../math/animatedtransform.h"
#include "cobject.h"

RENDERING_BEGIN

// 相机样本
struct CameraSample {
    // 在胶片上采样的随机数
    Point2f pFilm;
    // 在透镜上采样的随机数
    Point2f pLens;
    // 采样时间
    Float time;
    // 在filter上的权重
    Float weight;
};

class Camera {
public:
    Camera(const AnimatedTransform& CameraToWorld, Float shutterOpen,
        Float shutterClose, std::shared_ptr<Film> film, const Medium* medium);

    // 表示有多少沿着该光线有多少radiance到达相平面上
    virtual Float generateRay(const CameraSample& sample, Ray* ray) const = 0;

    virtual Float generateRayDifferential(const CameraSample& sample,
        RayDifferential* rd) const;

    virtual Spectrum We(const Ray& ray, Point2f* pRaster2 = nullptr) const;

    virtual void pdf_We(const Ray& ray, Float* pdfPos, Float* pdfDir) const;

    virtual Spectrum sample_Wi(const Interaction& ref, const Point2f& u,
        Vector3f* wi, Float* pdf, Point2f* pRaster,
        VisibilityTester* vis) const;

    // 相机空间到世界空间的转换，用animatedTransform可以做动态模糊
    AnimatedTransform cameraToWorld;
    // 快门开启时间，快门关闭时间
    const Float shutterOpen, shutterClose;
    // 胶片
    std::shared_ptr<Film> film;
    // 相机所在的介质
    const Medium* medium;
};

class ProjectiveCamera : public Camera {
public:
    ProjectiveCamera(const AnimatedTransform& CameraToWorld,
        const Transform& cameraToScreen,
        const AABB2f& screenWindow, Float shutterOpen,
        Float shutterClose, Float lensr, Float focalDistance, std::shared_ptr<Film> film,
        const Medium* medium)
        : Camera(CameraToWorld, shutterOpen, shutterClose, film, medium),
        _cameraToScreen(cameraToScreen) {

        _lensRadius = lensr;
        _focalDistance = focalDistance;

        // 屏幕空间(0,0)为胶片平面矩形的中点
        _screenToRaster = Transform::scale(film->fullResolution.x, film->fullResolution.y, 1)
            * Transform::scale(1 / (screenWindow.pMax.x - screenWindow.pMin.x),
                1 / (screenWindow.pMin.y - screenWindow.pMax.y), 1)
            * Transform::translate(Vector3f(-screenWindow.pMin.x, -screenWindow.pMax.y, 0));
        _rasterToScreen = _screenToRaster.getInverse();
        _rasterToCamera = _cameraToScreen.getInverse() * _rasterToScreen;
    }

    Transform getCameraToScreen() const {
        return _cameraToScreen;
    }

protected:
    Transform _cameraToScreen, _rasterToCamera;
    Transform _screenToRaster, _rasterToScreen;

    // 透镜半径
    Float _lensRadius;
    //通常 _focalDistance 由两个参数决定，透镜的焦距，以及film到透镜的实际距离
    Float _focalDistance;
};

RENDERING_END