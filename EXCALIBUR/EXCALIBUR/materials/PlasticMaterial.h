#pragma once

#include "../core/Header.h"
#include "../core/Material.h"
#include "../core/BXDF/BSDF.h"
#include "../core/BXDF/Lambertian.h"
#include "../parallel/Parallel.h"

RENDERING_BEGIN

class PlasticMaterial : public Material {
public:
    PlasticMaterial(const std::shared_ptr<Texture<Spectrum>>& Kd,
        const std::shared_ptr<Texture<Spectrum>>& Ks,
        const std::shared_ptr<Texture<Float>>& roughness,
        const std::shared_ptr<Texture<Float>>& bumpMap,
        bool remapRoughness)
        : Kd(Kd), Ks(Ks), roughness(roughness), bumpMap(bumpMap),
        remapRoughness(remapRoughness) { }

    virtual void ComputeScatteringFunctions(SurfaceInteraction* si,
        MemoryArena& arena,
        TransportMode mode,
        bool allowMultipleLobes) const override;
private:
    std::shared_ptr<Texture<Spectrum>> Kd, Ks;
    std::shared_ptr<Texture<Float>> roughness, bumpMap;
    const bool remapRoughness;
};

RENDERING_END