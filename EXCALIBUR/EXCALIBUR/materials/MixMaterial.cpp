#include "MixMaterial.h"
#include "../core/texture.h"

RENDERING_BEGIN

void MixMaterial::ComputeScatteringFunctions(SurfaceInteraction* si,
    MemoryArena& arena,
    TransportMode mode,
    bool allowMultipleLobes) const {
    Spectrum s1 = scale->evaluate(*si).clamp();
    Spectrum s2 = (Spectrum(1.f) - s1).clamp();
    SurfaceInteraction si2 = *si;
    m1->ComputeScatteringFunctions(si, arena, mode, allowMultipleLobes);
    m2->ComputeScatteringFunctions(&si2, arena, mode, allowMultipleLobes);

    int n1 = si->bsdf->numComponents(), n2 = si2.bsdf->numComponents();
    for (int i = 0; i < n1; ++i)
        si->bsdf->bxdfs[i] =
        ARENA_ALLOC(arena, ScaledBxDF)(si->bsdf->bxdfs[i], s1);

    for (int i = 0; i < n2; ++i)
        si->bsdf->add(ARENA_ALLOC(arena, ScaledBxDF)(si2.bsdf->bxdfs[i], s2));
}

RENDERING_END