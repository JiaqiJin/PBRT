#include "Interaction.h"

RENDERING_BEGIN

Ray Interaction::spawnRay(const Vector3f& d, bool forward) const {
    Normal3f n = forward ?
        (dot(d, normal) > 0 ? normal : -normal) :
        normal;
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

RENDERING_END