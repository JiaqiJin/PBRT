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

};

RENDERING_END