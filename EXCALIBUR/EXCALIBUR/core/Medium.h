#pragma once

#include "Header.h"

RENDERING_BEGIN

class Medium {
public:
    // Medium Interface
    virtual ~Medium() {}

    virtual Spectrum Tr(const Ray& ray, Sampler& sampler) const = 0;

    virtual Spectrum sample(const Ray& ray, Sampler& sampler,
        MemoryArena& arena,
        MediumInteraction* mi) const = 0;
};

struct MediumInterface {
    MediumInterface() : inside(nullptr), outside(nullptr) {}

    MediumInterface(const Medium* medium) : inside(medium), outside(medium) {

    }

    MediumInterface(const Medium* inside, const Medium* outside)
        :inside(inside),
        outside(outside) {
    }

    bool isMediumTransition() const {
        return inside != outside;
    }
    // 内部的介质
    const Medium* inside;

    // 外部的介质
    const Medium* outside;
};

RENDERING_END