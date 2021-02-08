#include "mirror.hpp"
#include "interaction.hpp"
#include "bxdf.hpp"
#include "texture.hpp"

PALADIN_BEGIN

void MirrorMaterial::computeScatteringFunctions(SurfaceInteraction* si,
    MemoryArena& arena,
    TransportMode mode,
    bool allowMultipleLobes) const {
    if (_bumpMap) {
        bump(_bumpMap, si);
    }
    si->bsdf = ARENA_ALLOC(arena, BSDF)(*si);
    Spectrum R = _Kr->evaluate(*si).Clamp();
    if (!R.IsBlack()) {
        FresnelNoOp* fresnel = ARENA_ALLOC(arena, FresnelNoOp)();
        SpecularReflection* sr = ARENA_ALLOC(arena, SpecularReflection)(R, fresnel);
        si->bsdf->add(sr);
    }

}

PALADIN_END