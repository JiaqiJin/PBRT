﻿#ifndef camera_hpp
#define camera_hpp

#include "header.h"
#include "film.hpp"
#include "../math/animatedtransform.hpp"

PALADIN_BEGIN

/*
 相机空间（camera space）：以相机在世界空间中的位置为原点，相机面朝的方向为 z 轴正方向展开的三维空间。

 屏幕空间（screen space）：屏幕空间定义在像平面（也可以说是近平面,胶片平面）上，屏幕空间中近平面所在矩形中点的坐标为 (0,0)
 ，坐标轴单位长度由长和宽中较短的边决定。相机空间物体将被投射到像平面上，而在屏幕窗口内可见的部分最终会出现在图像中。
 在屏幕空间下，深度值 z的范围是 [0,1] ，0和1分别对应了近平面和远平面上的物体。
 尽管屏幕空间是定义在像平面上的，但它仍然是一个三维空间，因为在该空间下坐标分量 z 仍然是有意义的
 如果是正方形的图片屏幕空间中xy的范围均为[-1,1]，如果是长方形的图片，则较短的一个维度映射到[-1,1]

 标准化设备坐标空间（normalized device coordinate space，NDC space）:xyz三个维度都在[0,1]范围内，(0,0)为左上角

 光栅空间（raster space）：该空间与NDC空间基本相同，z范围是[0,1]，唯一的区别 x范围是[0,res.x]，y范围是[0,res.y]
 */
    class Camera {
    public:
        Camera(const AnimatedTransform& CameraToWorld, Float shutterOpen,
            Float shutterClose, Film* film, const Medium* medium);

        virtual ~Camera() {
            delete film;
        }

        /*
         生成的ray中dir分量为单位向量
         返回值为一个浮点数，表示有多少沿着该光线有多少radiance到达相平面上
         一般返回值为0或1，真实相机中可能返回其他值
         */
        virtual Float generateRay(const CameraSample& sample, Ray* ray) const = 0;

        /*
         返回值为一个浮点数，表示有多少沿着该光线有多少radiance到达相平面上
         一般返回值为0或1，真实相机中可能返回其他值
         */
        virtual Float generateRayDifferential(const CameraSample& sample,
            RayDifferential* rd) const;

        virtual Spectrum we(const Ray& ray, Point2f* pRaster2 = nullptr) const;

        virtual void pdfWe(const Ray& ray, Float* pdfPos, Float* pdfDir) const;

        virtual Spectrum sampleWi(const Interaction& ref, const Point2f& u,
            Vector3f* wi, Float* pdf, Point2f* pRaster,
            VisibilityTester* vis) const;

        // 相机空间到世界空间的转换，用animatedTransform可以做动态模糊
        AnimatedTransform cameraToWorld;
        // 快门开启时间，快门关闭时间
        const Float shutterOpen, shutterClose;
        // 胶片
        Film* film;
        // 相机所在的介质
        const Medium* medium;
};

// 相机样本
struct CameraSample {
    // 在胶片上采样的随机数
    Point2f pFilm;
    // 在透镜上采样的随机数
    Point2f pLens;
    // 采样时间
    Float time;
};

/*
关于screenWindow，可以参考pbrt源码src/cameras/perspective.cpp第240-254行，
首先计算宽高比，然后将长和宽中较短的一边映射到 [-1,1] 并保持整个矩形的宽高比不变
 */
class ProjectiveCamera : public Camera {
public:

    ProjectiveCamera(const AnimatedTransform& CameraToWorld,
        const Transform& cameraToScreen,
        const AABB2f& screenWindow, Float shutterOpen,
        Float shutterClose, Float lensr, Float focalDistance, Film* film,
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

protected:

    Transform _cameraToScreen, _rasterToCamera;
    Transform _screenToRaster, _rasterToScreen;

    // 透镜半径
    Float _lensRadius;

    // 存在一个平面，如果一个点在该平面上
    // 无论透镜的样本点在哪里，点在film上呈的像都不会模糊
    // 我暂时把该平面称为film所对应的聚焦平面（简称聚焦平面），聚焦平面与透镜中心的距离定义为 _focalDistance
    // 通常 _focalDistance 由两个参数决定，透镜的焦距，以及film到透镜的实际距离
    // 为了方便计算，我们省略了以上两个中间参数，直接定义了_focalDistance
    Float _focalDistance;
};

PALADIN_END

#endif /* camera_hpp */