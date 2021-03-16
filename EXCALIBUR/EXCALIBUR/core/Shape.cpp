﻿#include "Shape.h"
#include "../math/lowdiscrepancy.h"

RENDERING_BEGIN

Shape::~Shape() {

}

Shape::Shape(const Transform* objectToWorld, const Transform* worldToObject,
    bool reverseOrientation)
    : objectToWorld(objectToWorld),
    worldToObject(worldToObject),
    reverseOrientation(reverseOrientation),
    transformSwapsHandedness(objectToWorld->swapsHandedness()) {

}

AABB3f Shape::worldBound() const {
    return objectToWorld->exec(objectBound());
}

/*
由立体角定义 dw/dA = cosθ/r^2
联合表达式 py(y) * dy/dx = px(x)
可得 p(w) = r^2 / cosθ * p(A)
 p(A) = 1/A
 p(w) = r^2 / (A cosθ)
 */
Interaction Shape::sampleW(const Interaction& ref, const Point2f& u, Float* pdf) const {
    Interaction intr = sampleA(u, pdf);
    Vector3f wi = intr.pos - ref.pos;
    if (wi.lengthSquared() == 0) {
        *pdf = 0.f;
    }
    else {
        wi = normalize(wi);
        *pdf *= distanceSquared(ref.pos, intr.pos) / absDot(intr.normal, -wi);
        if (std::isinf(*pdf)) {
            *pdf = 0.f;
        }
    }
    return intr;
}

/*
  p(w) = r^2 / cosθ * p(A)
  p(w) = r^2 / (A cosθ)
  这个接口通常是shape作为光源时，场景中某个interaction对光源采样的概率密度函数
  函数空间为向量空间
 */
Float Shape::pdfW(const Interaction& ref, const Vector3f& wi) const {
    Ray ray = ref.spawnRay(wi);
    Float tHit;
    SurfaceInteraction isect;
    if (!intersect(ray, &tHit, &isect, false)) {
        return 0;
    }

    Float pdf = distanceSquared(isect.pos, ref.pos) / (absDot(wi, isect.normal) * area());
    if (std::isinf(pdf)) {
        pdf = 0.f;
    }
    return pdf;
}

/*
 基类获取立体角的方式，暴力低差异采样，三角形跟球体有简单的方式
 */
Float Shape::solidAngle(const Point3f& p, int nSamples) const {
    Interaction ref(p, Normal3f(), Vector3f(), Vector3f(0, 0, 1), 0,
        MediumInterface{});
    double solidAngle = 0;
    for (int i = 0; i < nSamples; ++i) {
        Point2f u{ RadicalInverse(0, i), RadicalInverse(1, i) };
        Float pdf;
        Interaction pShape = sampleW(ref, u, &pdf);
        if (pdf > 0 && !intersectP(Ray(p, pShape.pos - p, .999f))) {
            solidAngle += 1 / pdf;
        }
    }
    return solidAngle / nSamples;
}

RENDERING_END