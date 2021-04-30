#include "Material.h"

#include "BSDF.h"
#include "../Tool/Memory.h"
#include "Spectrum.h"
#include "Interaction.h"

RENDER_BEGIN

void MirrorMaterial::computeScatteringFunctions(SurfaceInteraction& si, MemoryArena& arena,
	TransportMode mode, bool allowMultipleLobes) const
{
	si.bsdf = ARENA_ALLOC(arena, BSDF)(si);
	Spectrum R = m_Kr;
	if (!R.isBlack())
	{
		si.bsdf->add(ARENA_ALLOC(arena, SpecularReflection)(
			R, ARENA_ALLOC(arena, FresnelNoOp)()));
	}
}

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