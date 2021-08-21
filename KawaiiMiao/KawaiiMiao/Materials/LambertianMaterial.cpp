#include "LambertianMaterial.h"

#include "../Core/BSDF.h"
#include "../Tool/Memory.h"
#include "../Core/Spectrum.h"
#include "../Core/Interaction.h"

RENDER_BEGIN

void LambertianMaterial::computeScatteringFunctions(SurfaceInteraction& si, MemoryArena& arena,
	TransportMode mode, bool allowMultipleLobes) const
{
	si.bsdf = ARENA_ALLOC(arena, BSDF)(si);
	Spectrum R = m_Kr;
	if (!R.isBlack())
	{
		si.bsdf->add(ARENA_ALLOC(arena, LambertianReflection)(R));
	}
}

RENDER_END