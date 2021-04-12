#pragma once

#include "Header.h"
#include "Interaction.h"
#include "../math/transform.h"

RENDERING_BEGIN

enum class LightFlags {
    // 点光源
    DeltaPosition = 1,
    // 方向光，只有一个方向
    DeltaDirection = 2,
    // 面光源
    Area = 4,
    Infinite = 8
};

inline bool isDeltaLight(int flags) {
    return flags & (int)LightFlags::DeltaPosition ||
        flags & (int)LightFlags::DeltaDirection;
}

STAT_COUNTER("Scene/Lights", numLights);
STAT_COUNTER("Scene/AreaLights", numAreaLights);

class Light {
public:
    virtual ~Light() { }

    Light(int flags, const Transform& LightToWorld,
        const MediumInterface& mediumInterface, int nSamples = 1)
        : flags(flags),
        nSamples(std::max(1, nSamples)),
        mediumInterface(mediumInterface),
        _lightToWorld(LightToWorld),
        _worldToLight(LightToWorld.getInverse()) {
        ++numLights;
    }

    inline bool isDelta() const {
        return isDeltaLight(flags);
    }


    // LightFlags
    const int flags;
    // 为了计算soft shadow的采样数量
    const int nSamples;

    const MediumInterface mediumInterface;

protected:
    const Transform _lightToWorld, _worldToLight;
};

RENDERING_END