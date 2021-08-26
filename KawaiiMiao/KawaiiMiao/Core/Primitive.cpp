#include "Primitive.h"

#include "Interaction.h"

RENDER_BEGIN

HitableObject::HitableObject(const Shape::ptr& shape, const Material::ptr& material,
	const AreaLight::ptr& areaLight)
	: m_shape(shape), m_material(material), m_areaLight(areaLight) 
{
	if (m_areaLight != nullptr)
	{
		m_areaLight->setParent(this);
	}
}

bool HitableObject::hit(const Ray& ray) const { return m_shape->hit(ray); }

bool HitableObject::hit(const Ray& ray, SurfaceInteraction& isect) const
{
	Float tHit;
	if (!m_shape->hit(ray, tHit, isect))
		return false;

	ray.m_tMax = tHit;
	isect.hitable = this;
	return true;
}

void HitableObject::computeScatteringFunctions(SurfaceInteraction& isect, MemoryArena& arena,
	TransportMode mode, bool allowMultipleLobes) const
{
	if (m_material != nullptr)
	{
		m_material->computeScatteringFunctions(isect, arena, mode, allowMultipleLobes);
	}
}

Bounds3f HitableObject::worldBound() const { return m_shape->worldBound(); }

const AreaLight* HitableObject::getAreaLight() const { return m_areaLight.get(); }

const Material* HitableObject::getMaterial() const { return m_material.get(); }

// ------------------------ Aggregate ---------------------------------
const AreaLight* HitableAggregate::getAreaLight() const { return nullptr; }

const Material* HitableAggregate::getMaterial() const { return nullptr; }

void HitableAggregate::computeScatteringFunctions(SurfaceInteraction& isect, MemoryArena& arena,
	TransportMode mode, bool allowMultipleLobes) const
{
	//Note: should not go here at all.
	std::cout << "AHitableAggregate::computeScatteringFunctions() shouldn't be ""called";
}

RENDER_END