#include "PlasticMaterial.h"
#include "../core/texture.h"
#include "../core/Interaction.h"
#include "../core/BXDF/BxDF.h"
#include "../core/spectrum.h"
#include "../core/BXDF/microfacete/MicrofacetReflection.h"

RENDERING_BEGIN

void PlasticMaterial::ComputeScatteringFunctions(SurfaceInteraction* si,
    MemoryArena& arena,
    TransportMode mode,
    bool allowMultipleLobes) const
{
    if (bumpMap)
        Bump(bumpMap, si);

    si->bsdf = ARENA_ALLOC(arena, BSDF)(*si);
    Spectrum kd = Kd->evaluate(*si).clamp();
    if (!kd.IsBlack()) {
        si->bsdf->add(ARENA_ALLOC(arena, LambertianReflection)(kd));
    }
    Spectrum ks = Ks->evaluate(*si).clamp();
    if (!ks.IsBlack()) {
        Fresnel* fresnel = ARENA_ALLOC(arena, FresnelDielectric)(1.f, 1.5f);
        // Create microfacet distribution _distrib_ for plastic material
        Float rought = roughness->evaluate(*si);
        if (remapRoughness)
            rought = TrowbridgeReitzDistribution::RoughnessToAlpha(rought);

        MicrofacetDistribution* distrib =
            ARENA_ALLOC(arena, TrowbridgeReitzDistribution)(rought, rought);

        BxDF* spec =
            ARENA_ALLOC(arena, MicrofacetReflection)(ks, distrib, fresnel);
        si->bsdf->add(spec);
    }
}

RENDERING_END