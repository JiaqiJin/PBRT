#include "matte.hpp"
#include "bxdf.hpp"
#include "interaction.hpp"
#include "texture.hpp"

PALADIN_BEGIN

void MatteMaterial::computeScatteringFunctions(SurfaceInteraction* si,
    MemoryArena& arena,
    TransportMode mode,
    bool allowMultipleLobes) const {
	if (_bumpMap) {
		bump(_bumpMap, si);
	}

	si->bsdf = ARENA_ALLOC(arena, BSDF)(*si);
	Spectrum r = _Kd->evaluate(*si).Clamp();
	Float sig = clamp(_sigma->evaluate(*si), 0, 90);
	if (!r.IsBlack()) {
		if (sig == 0) {
			// 如果粗糙度为零，朗博反射
			si->bsdf->add(ARENA_ALLOC(arena, LambertianReflection)(r));
		}
		else {
			si->bsdf->add(ARENA_ALLOC(arena, OrenNayar)(r, sig));
		}
	}
}

PALADIN_END