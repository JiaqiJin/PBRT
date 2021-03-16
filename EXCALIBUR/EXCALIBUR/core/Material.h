#pragma once

#include "Header.h"

RENDERING_BEGIN

class Material {
public:
    // Material Interface
    virtual void ComputeScatteringFunctions(SurfaceInteraction* si,
        MemoryArena& arena,
        TransportMode mode,
        bool allowMultipleLobes) const = 0;
    virtual ~Material();


};

RENDERING_END