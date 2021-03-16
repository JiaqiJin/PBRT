#include "Interaction.h"
#include "Shape.h"

RENDERING_BEGIN

Ray Interaction::spawnRay(const Vector3f& d, bool forward) const {
    Normal3f n = forward ?
        (dot(d, normal) > 0 ? normal : -normal) : normal;
    Point3f o = offsetRay(pos, n, d);
    return Ray(o, d, Infinity, time, getMedium(d));
}

Ray Interaction::spawnRayTo(const Point3f& p2) const {
    Point3f origin = offsetRay(pos, normal, p2 - pos);
    Vector3f d = p2 - pos;
    // 这里的tMax为1，因为真实的长度已经在d方向里保存，又因为光线的终点不能在p2点上，所以。。。
    return Ray(origin, d, 1 - ShadowEpsilon, time, getMedium(d));
}

Ray Interaction::spawnRayTo(const Interaction& it) const {
    Point3f origin = offsetRay(pos, normal, it.pos - pos);
    Point3f target = offsetRay(it.pos, it.normal, origin - it.pos);
    Vector3f d = target - origin;
    return Ray(origin, d, 1 - ShadowEpsilon, time, getMedium(d));
}

SurfaceInteraction::SurfaceInteraction(
    const Point3f& p, const Vector3f& pError, const Point2f& uv,
    const Vector3f& wo, const Vector3f& dpdu, const Vector3f& dpdv,
    const Normal3f& dndu, const Normal3f& dndv, Float time, const Shape* shape,
    int faceIndex)
    : Interaction(p, Normal3f(normalize(cross(dpdu, dpdv))), pError, wo, time, nullptr),
    uv(uv),
    dpdu(dpdu),
    dpdv(dpdv),
    dndu(dndu),
    dndv(dndv),
    shape(shape),
    faceIndex(faceIndex) {

    shading.normal = normal;
    shading.dpdu = dpdu;
    shading.dpdv = dpdv;
    shading.dndu = dndu;
    shading.dndv = dndv;

    if (shape &&
        (shape->reverseOrientation ^ shape->transformSwapsHandedness)) {
        normal *= -1;
        shading.normal *= -1;
    }
}

void SurfaceInteraction::setShadingGeometry(const Vector3f& dpdus,
    const Vector3f& dpdvs,
    const Normal3f& dndus,
    const Normal3f& dndvs,
    bool orientationIsAuthoritative) {

    shading.normal = normalize((Normal3f)cross(dpdus, dpdvs));

    if (shape && (shape->reverseOrientation ^ shape->transformSwapsHandedness)) {
        shading.normal = -shading.normal;
    }

    if (orientationIsAuthoritative) {
        normal = faceforward(normal, shading.normal);
    }
    else {
        shading.normal = faceforward(shading.normal, normal);
    }

    shading.dpdu = dpdus;
    shading.dpdv = dpdvs;
    shading.dndu = dndus;
    shading.dndv = dndvs;
}

RENDERING_END