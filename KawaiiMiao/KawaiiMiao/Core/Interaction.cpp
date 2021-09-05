#include "Interaction.h"
#include "Spectrum.h"
#include "Primitive.h"
#include "BSDF.h"
#include "Shape.h"

RENDER_BEGIN

SurfaceInteraction::SurfaceInteraction(const Vector3f& p, const Vector2f& uv, const Vector3f& wo,
	const Vector3f& dpdu, const Vector3f& dpdv, const Shape* sh)
	: Interaction(p, normalize(cross(dpdu, dpdv)), wo), uv(uv), dpdu(dpdu), dpdv(dpdv), shape(sh) 
{

}

SurfaceInteraction::SurfaceInteraction(const Vector3f& p, const Vector3f& pError,
	const Vector2f& uv, const Vector3f& wo,
	const Vector3f& dpdu, const Vector3f& dpdv,
	const Vector3f& dndu, const Vector3f& dndv, Float time,
	const Shape* sh,
	int faceIndex) :
    Interaction(p, Vector3f(normalize(cross(dpdu, dpdv))), pError, wo, time,
        nullptr),
    uv(uv),
    dpdu(dpdu),
    dpdv(dpdv),
    dndu(dndu),
    dndv(dndv),
    shape(shape),
    faceIndex(faceIndex) {
    // Initialize shading geometry from true geometry
    shading.n = normal;
    shading.dpdu = dpdu;
    shading.dpdv = dpdv;
    shading.dndu = dndu;
    shading.dndv = dndv;

    // Adjust normal based on orientation and handedness
    if (shape && (shape->reverseOrientation ^ shape->transformSwapsHandedness))
    {
        normal *= -1;
        shading.n *= -1;
    }
}

Spectrum SurfaceInteraction::Le(const Vector3f& w) const
{
	const AreaLight* area = primitive->getAreaLight();
	return area != nullptr ? area->L(*this, w) : Spectrum(0.f);
}

void SurfaceInteraction::computeScatteringFunctions(const Ray& ray, MemoryArena& arena,
	bool allowMultipleLobes, TransportMode mode)
{
	primitive->computeScatteringFunctions(*this, arena, mode, allowMultipleLobes);
}

RENDER_END