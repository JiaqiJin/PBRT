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
    //  bump函数，用于更新SurfaceInteraction变量 计算对应点的偏移值并更新
    static void Bump(const std::shared_ptr<Texture<Float>>& d, SurfaceInteraction* si);
    virtual ~Material() {}
};

RENDERING_END