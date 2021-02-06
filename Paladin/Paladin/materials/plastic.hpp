#ifndef plastic_hpp
#define plastic_hpp

#include "material.hpp"

PALADIN_BEGIN

class PlasticMaterial : Material {
public:
    PlasticMaterial(const std::shared_ptr<Texture<Spectrum>>& Kd,
        const std::shared_ptr<Texture<Spectrum>>& Ks,
        const std::shared_ptr<Texture<Float>>& roughness,
        const std::shared_ptr<Texture<Float>>& bumpMap,
        bool remapRoughness)
        :_Kd(Kd),
        _Ks(Ks),
        _roughness(roughness),
        _bumpMap(bumpMap),
        _remapRoughness(remapRoughness) {}

    virtual void computeScatteringFunctions(SurfaceInteraction* si, MemoryArena& arena,
        TransportMode mode,
        bool allowMultipleLobes) const;
private:
    std::shared_ptr<Texture<Spectrum>> _Kd, _Ks;
    std::shared_ptr<Texture<Float>> _roughness, _bumpMap;
    const bool _remapRoughness;
};

PALADIN_END

#endif