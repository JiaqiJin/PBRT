﻿#include "glass.hpp"
#include "interaction.hpp"
#include "bxdf.hpp"
#include "texture.hpp"

PALADIN_BEGIN

void GlassMaterial::computeScatteringFunctions(SurfaceInteraction* si,
    MemoryArena& arena,
    TransportMode mode,
    bool allowMultipleLobes) const {
    if (_bumpMap) {
        bump(_bumpMap, si);
    }
    Float eta = _eta->evaluate(*si);
    Float urough = _uRoughness->evaluate(*si);
    Float vrough = _vRoughness->evaluate(*si);
    Spectrum R = _Kr->evaluate(*si).Clamp();
    Spectrum T = _Kt->evaluate(*si).Clamp();

    si->bsdf = ARENA_ALLOC(arena, BSDF)(*si, eta);

    if (R.IsBlack() && T.IsBlack()) {
        return;
    }

    bool isSpecular = urough == 0 && vrough == 0;

    Float etaMedium = 1.0f;

    if (isSpecular && allowMultipleLobes) {
        // 这里的介质折射率不应该固定为1.0f，应该根据介质  todo
        FresnelSpecular* fr = ARENA_ALLOC(arena, FresnelSpecular)(R, T, etaMedium, eta, mode);
        si->bsdf->add(fr);
    }
    else {
        if (_remapRoughness) {
            urough = TrowbridgeReitzDistribution::RoughnessToAlpha(urough);
            vrough = TrowbridgeReitzDistribution::RoughnessToAlpha(vrough);
        }

        MicrofacetDistribution* distrib = isSpecular ?
            nullptr :
            ARENA_ALLOC(arena, TrowbridgeReitzDistribution)(urough, vrough);
        if (!R.IsBlack()) {
            Fresnel* fresnel = ARENA_ALLOC(arena, FresnelDielectric)(etaMedium, eta);
            if (isSpecular) {
                BxDF* bxdf = ARENA_ALLOC(arena, SpecularReflection)(R, fresnel);
                si->bsdf->add(bxdf);
            }
            else {
                BxDF* bxdf = ARENA_ALLOC(arena, MicrofacetReflection)(R, distrib, fresnel);
                si->bsdf->add(bxdf);
            }
        }
        if (!T.IsBlack()) {
            if (isSpecular) {
                BxDF* bxdf = ARENA_ALLOC(arena, SpecularTransmission)(T, etaMedium, eta, mode);
                si->bsdf->add(bxdf);
            }
            else {
                BxDF* bxdf = ARENA_ALLOC(arena, MicrofacetTransmission)(T, distrib, etaMedium, eta, mode);
                si->bsdf->add(bxdf);
            }
        }
    }
}



PALADIN_END
