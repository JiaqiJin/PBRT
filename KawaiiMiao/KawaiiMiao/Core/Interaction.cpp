#include "Interaction.h"
#include "Spectrum.h"
#include "Primitive.h"
#include "BSDF.h"

RENDER_BEGIN

SurfaceInteraction::SurfaceInteraction(const Vector3f& p, const Vector2f& uv, const Vector3f& wo,
	const Vector3f& dpdu, const Vector3f& dpdv, const Shape* sh)
	: Interaction(p, normalize(cross(dpdu, dpdv)), wo), uv(uv), dpdu(dpdu), dpdv(dpdv), shape(sh) 
{

}

Spectrum SurfaceInteraction::Le(const Vector3f& w) const
{
	return Spectrum(1.0f);
}

void SurfaceInteraction::computeScatteringFunctions(const Ray& ray, MemoryArena& arena,
	bool allowMultipleLobes, TransportMode mode)
{
	hitable->computeScatteringFunctions(*this, arena, mode, allowMultipleLobes);
}

RENDER_END