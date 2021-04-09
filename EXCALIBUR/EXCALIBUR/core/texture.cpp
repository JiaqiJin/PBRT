#include "texture.h"

RENDERING_BEGIN

Point2f UVMapping2D::map(const SurfaceInteraction& si, Vector2f* dstdx, Vector2f* dstdy) const {
    Float dsdx = su * si.dudx;
    // dt/dx = dt/dv * dv/dx
    Float dtdx = sv * si.dvdx;
    *dstdx = Vector2f(dsdx, dtdx);

    Float dsdy = su * si.dudy;
    Float dtdy = sv * si.dvdy;
    *dstdy = Vector2f(dsdy, dtdy);

    return Point2f(su * si.uv[0] + du,
        sv * si.uv[1] + dv);
}

//  ds        fs(p + △x dp/dx) - fs(p)
// ---- ≈ ---------------------------------
//  dx                 △x
Point2f SphericalMapping2D::map(const SurfaceInteraction& si, Vector2f* dstdx, Vector2f* dstdy) const {
    Point2f st = pointToSphereToST(si.pos);
    const Float delta = 0.1f;

    Point2f stDeltaX = pointToSphereToST(si.pos + delta * si.dpdx);
    *dstdx = (stDeltaX - st) / delta;
 
    if ((*dstdx)[1] > 0.5f) {
        // 0到2π
        (*dstdx)[1] = 1 - (*dstdx)[1];
    }
    else if ((*dstdx)[1] < -0.5f) {
        // 2π到0
        (*dstdx)[1] = -((*dstdx)[1] + 1);
    }

    Point2f stDeltaY = pointToSphereToST(si.pos + delta * si.dpdy);
    *dstdy = (stDeltaY - st) / delta;
    if ((*dstdy)[1] > 0.5f) {
        (*dstdy)[1] = 1 - (*dstdy)[1];
    }
    else if ((*dstdy)[1] < -0.5f) {
        (*dstdy)[1] = -((*dstdy)[1] + 1);
    }

    return st;
}

Point2f SphericalMapping2D::pointToSphereToST(const Point3f& p) const {
    Vector3f vec = normalize(_worldToTexture.exec(p) - Point3f(0, 0, 0));
    Float theta = sphericalTheta(vec);
    Float phi = sphericalPhi(vec);
    return Point2f(theta * InvPi, phi * Inv2Pi);
}

Point3f TransformMapping3D::map(const SurfaceInteraction& si, Vector3f* dpdx,
    Vector3f* dpdy) const {
    *dpdx = _worldToTexture.exec(si.dpdx);
    *dpdy = _worldToTexture.exec(si.dpdy);
    return _worldToTexture.exec(si.pos);
}

Float lanczos(Float x, Float tau) {
    x = std::abs(x);
    if (x < 1e-5f) {
        return 1;
    }
    if (x > 1.f) {
        return 0;
    }
    x *= Pi;
    Float s = std::sin(x * tau) / (x * tau);
    Float lanczos = std::sin(x) / x;
    return s * lanczos;
}

RENDERING_END