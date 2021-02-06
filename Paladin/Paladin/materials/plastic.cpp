#include "plastic.hpp"
#include "bxdf.hpp"
#include "spectrum.hpp"
#include "texture.hpp"

PALADIN_BEGIN

void PlasticMaterial::computeScatteringFunctions(
    SurfaceInteraction* si, MemoryArena& arena, TransportMode mode,
    bool allowMultipleLobes) const {

    if (_bumpMap) {
        bump(_bumpMap, si);
    }
    si->bsdf = ARENA_ALLOC(arena, BSDF)(*si);
    Spectrum kd = _Kd->evaluate(*si).Clamp();
    if (!kd.IsBlack()) {
        si->bsdf->add(ARENA_ALLOC(arena, LambertianReflection)(kd));
    }
  
    Spectrum ks = _Ks->evaluate(*si).Clamp();
    if (!ks.IsBlack()) {
        Fresnel* fresnel = ARENA_ALLOC(arena, FresnelDielectric)(1.5f, 1.f);
        //Create microfacet distribution distrib for plastic material
        Float rough = _roughness->evaluate(*si);
        if (_remapRoughness) {
            rough = TrowbridgeReitzDistribution::RoughnessToAlpha(rough);
        }
        MicrofacetDistribution* distrib =
            ARENA_ALLOC(arena, TrowbridgeReitzDistribution)(rough, rough);
        BxDF* spec =
            ARENA_ALLOC(arena, MicrofacetReflection)(ks, distrib, fresnel);
        si->bsdf->add(spec);
    }
}

PALADIN_END