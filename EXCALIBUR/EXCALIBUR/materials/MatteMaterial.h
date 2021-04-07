#pragma once

#include "../core/Header.h"
#include "../core/Material.h"
#include "../core/BXDF/BSDF.h"
#include "../core/BXDF/Lambertian.h"
#include "../parallel/Parallel.h"

RENDERING_BEGIN

class MatteMaterial : public Material {
public:
    MatteMaterial(const std::shared_ptr<Texture<Spectrum>>& Kd,
        const std::shared_ptr<Texture<Float>>& sigma,
        const std::shared_ptr<Texture<Float>>& bumpMap)
        : _Kd(Kd), _sigma(sigma), _bumpMap(bumpMap) { }
    virtual void ComputeScatteringFunctions(SurfaceInteraction* si,
        MemoryArena& arena,
        TransportMode mode,
        bool allowMultipleLobes) const override;
private:
    // 漫反射系数
    std::shared_ptr<Texture<Spectrum>> _Kd;
    // 粗糙度
    std::shared_ptr<Texture<Float>> _sigma;
    // bump贴图
    std::shared_ptr<Texture<Float>> _bumpMap;
};

RENDERING_END