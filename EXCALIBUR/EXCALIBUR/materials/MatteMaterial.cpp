#include "MatteMaterial.h"
#include "../core/texture.h"
#include "../core/Interaction.h"
#include "../core/BXDF/BxDF.h"

RENDERING_BEGIN

void MatteMaterial::ComputeScatteringFunctions(SurfaceInteraction* si,
    MemoryArena& arena,
    TransportMode mode,
    bool allowMultipleLobes) const
{
    if (_bumpMap)
        Bump(_bumpMap, si);

    si->bsdf = ARENA_ALLOC(arena, BSDF)(*si);
    Spectrum r = _Kd->evaluate(*si).clamp();
    Float sig = clamp(_sigma->evaluate(*si), 0, 90);
    if (r.IsBlack()) {
        if (_sigma == 0) {
            // 如果粗糙度为零，朗博反射
            si->bsdf->add(ARENA_ALLOC(arena, LambertianReflection)(r));
        }
        else {
            si->bsdf->add(ARENA_ALLOC(arena, OrenNayar)(r,sig));
        }
    }
}

RENDERING_END