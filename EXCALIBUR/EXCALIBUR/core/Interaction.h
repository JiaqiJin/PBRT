#pragma once

#include "Header.h"
#include "Medium.h"

RENDERING_BEGIN

struct Interaction {
    Interaction() : time(0) {}

    Interaction(const Point3f& p, const Normal3f& n, const Vector3f& pError,
        const Vector3f& wo, Float time,
        const MediumInterface& mediumInterface)
        :pos(p),
        time(time),
        pError(pError),
        wo(normalize(wo)),
        normal(n),
        mediumInterface(mediumInterface) {

    }

    Interaction(const Point3f& p, const Vector3f& wo, Float time,
        const MediumInterface& mediumInterface)
        : pos(p),
        time(time),
        wo(wo),
        mediumInterface(mediumInterface) {

    }

    Interaction(const Point3f& p, const Normal3f& n, Float time = 0,
        const MediumInterface& mediumInterface = nullptr)
        : pos(p),
        time(time),
        normal(n),
        mediumInterface(mediumInterface) {

    }

    Interaction(const Point3f& p, Float time = 0,
        const MediumInterface& mediumInterface = nullptr)
        : pos(p),
        time(time),
        mediumInterface(mediumInterface) {

    }

    const Medium* getMedium(const Vector3f& w) const {
        return dot(w, normal) > 0 ? mediumInterface.outside : mediumInterface.inside;
    }

    const Medium* getMedium() const {
        CHECK_EQ(mediumInterface.inside, mediumInterface.outside);
        return mediumInterface.inside;
    }

    bool isSurfaceInteraction() const {
        return !normal.isZero();
    }

    bool isMediumInteraction() const {
        return !isSurfaceInteraction();
    }

    //  一般用于在指定交点处根据序列生成ray找到下一个交点
    Ray spawnRay(const Vector3f& d, bool forward = false) const;

    // 一般用于生成shadow ray，判断两个点之间是否有阻挡
    Ray spawnRayTo(const Point3f& p2) const;

    // 一般用于生成shadow ray，判断两个点之间是否有阻挡
    Ray spawnRayTo(const Interaction& it) const;

    Point3f pos;
    Float time;
    // 位置误差
    Vector3f pError;
    // 出射方向
    Vector3f wo;
    Normal3f normal;
    MediumInterface mediumInterface;
};

class SurfaceInteraction : public Interaction {
    // 用于着色的参数结构
    struct Shading {
        Normal3f normal;
        Vector3f dpdu, dpdv;
        Normal3f dndu, dndv;
    };
public:
    SurfaceInteraction() {}

    SurfaceInteraction(const Point3f& p, const Vector3f& pError,
        const Point2f& uv, const Vector3f& wo,
        const Vector3f& dpdu, const Vector3f& dpdv,
        const Normal3f& dndu, const Normal3f& dndv, Float time,
        const Shape* sh,
        int faceIndex = 0);

    void setShadingGeometry(const Vector3f& dpdu, const Vector3f& dpdv,
        const Normal3f& dndu, const Normal3f& dndv,
        bool orientationIsAuthoritative);

    void computeDifferentials(const RayDifferential& ray) const;

    void computeScatteringFunctions(const RayDifferential& ray,
        MemoryArena& arena,
        bool allowMultipleLobes = false,
        TransportMode mode = TransportMode::Radiance);

    inline void faceForward() {
        normal = faceforward(normal, shading.normal);
    }

    // 表面坐标
    Point2f uv;
    Vector3f dpdu, dpdv;
    Normal3f dndu, dndv;

    const Primitive* primitive = nullptr;
    const Shape* shape = nullptr;
    Shading shading;

    BSDF* bsdf = nullptr;
    BSSRDF* bssrdf = nullptr;

    mutable Vector3f dpdx, dpdy;

    mutable Float dudx = 0, dvdx = 0, dudy = 0, dvdy = 0;

    int faceIndex = 0;

    uint32_t shapeIdx;
    uint32_t primIdx;
};

class MediumInteraction : public Interaction {
public:
    MediumInteraction() : phase(nullptr) {

    }

    MediumInteraction(const Point3f& p, const Vector3f& wo, Float time,
        const Medium* medium, const PhaseFunction* phase)
        : Interaction(p, wo, time, medium), phase(phase) {

    }

    bool isValid() const {
        return phase != nullptr;
    }

    const PhaseFunction* phase;
};

RENDERING_END