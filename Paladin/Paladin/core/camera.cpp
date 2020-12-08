﻿#include "camera.hpp"

PALADIN_BEGIN

Camera::Camera(const AnimatedTransform& CameraToWorld, Float shutterOpen,
    Float shutterClose, Film* film, const Medium* medium)
    : cameraToWorld(CameraToWorld),
    shutterOpen(shutterOpen),
    shutterClose(shutterClose),
    film(film),
    medium(medium) {
    if (cameraToWorld.hasScale()) {

    }

}

Float Camera::generateRayDifferential(const CameraSample& sample, RayDifferential* rd) const {
    Float ret = generateRay(sample, rd);
    if (ret == 0) {
        return ret;
    }

    // 生成x方向辅助光线
    CameraSample sshift = sample;
    ++sshift.pFilm.x;
    Ray rx;
    Float retx = generateRay(sshift, &rx);
    // todo 暂时不理解为何要这样要返回0
    if (retx == 0) {
        return 0;
    }
    rd->rxOrigin = rx.ori;
    rd->rxDirection = rx.dir;

    // 生成y方向辅助光线
    --sshift.pFilm.x;
    ++sshift.pFilm.y;
    Ray ry;
    Float rety = generateRay(sshift, &ry);
    rd->ryOrigin = ry.ori;
    rd->ryDirection = ry.dir;
    if (rety == 0) {
        return 0;
    }

    return ret;
}


Spectrum Camera::we(const Ray& ray, Point2f* raster) const {
    COUT << "amera::We is not implemented.";
    assert(false);
    return Spectrum(0.f);
}

void Camera::pdfWe(const Ray& ray, Float* pdfPos, Float* pdfDir) const {
    COUT << "Camera::Pdf_We() is not implemented!";
    assert(false);
}

Spectrum Camera::sampleWi(const Interaction& ref, const Point2f& u,
    Vector3f* wi, Float* pdf, Point2f* pRaster,
    VisibilityTester* vis) const {
    COUT << "Camera::Sample_Wi() is not implemented!";
    assert(false);
    return Spectrum(0.f);
}

PALADIN_END
