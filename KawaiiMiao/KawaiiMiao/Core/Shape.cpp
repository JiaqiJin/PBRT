#include "Shape.h"

#include "Interaction.h"
#include "Sampling.h"
#include <array>

RENDER_BEGIN

Shape::Shape(Transform* objectToWorld, Transform* worldToObject)
	: m_objectToWorld(objectToWorld), m_worldToObject(worldToObject) 
{

}

bool Shape::hit(const Ray& ray) const
{
	Float tHit = ray.m_tMax;
	SurfaceInteraction isect;
	return hit(ray, tHit, isect);
}

Bounds3f Shape::worldBound() const
{ 
	return (*m_objectToWorld)(objectBound()); 
}

void Shape::setTransform(Transform* objectToWorld, Transform* worldToObject)
{
	m_objectToWorld = objectToWorld;
	m_worldToObject = worldToObject;
}

Interaction Shape::sample(const Interaction& ref, const Vector2f& u, Float& pdf) const
{
	// Sample a point on the shape given a reference point |ref| and
	// return the PDF with respect to solid angle from |ref|.
	Interaction intr = sample(u, pdf);
	Vector3f wi = intr.p - ref.p;
	if (dot(wi, wi) == 0)
	{
		pdf = 0;
	}
	else
	{
		wi = normalize(wi);
		// Convert from area measure, as returned by the Sample() call
		// above, to solid angle measure.
		pdf *= distanceSquared(ref.p, intr.p) / absDot(intr.normal, -wi);
		if (std::isinf(pdf))
			pdf = 0.f;
	}
	return intr;
}

Float Shape::pdf(const Interaction& ref, const Vector3f& wi) const
{
	// Intersect sample ray with area light geometry
	Ray ray = ref.spawnRay(wi);
	Float tHit;
	SurfaceInteraction isectLight;
	// Ignore any alpha textures used for trimming the shape when performing
	// this intersection. Hack for the "San Miguel" scene, where this is used
	// to make an invisible area light.
	if (!hit(ray, tHit, isectLight))
		return 0;

	// Convert light sample weight to solid angle measure
	Float pdf = distanceSquared(ref.p, isectLight.p) / (absDot(isectLight.normal, -wi) * area());
	if (std::isinf(pdf))
		pdf = 0.f;
	return pdf;
}

Float Shape::solidAngle(const Vector3f& p, int nSamples) const
{
	return 1.0f;
}

RENDER_END