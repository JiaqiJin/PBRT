#include "Primitive.h"

#include "Interaction.h"

RENDER_BEGIN

PrimitiveObject::PrimitiveObject(const Shape::ptr& shape, const Material* material,
	const AreaLight::ptr& areaLight)
	: m_shape(shape), m_material(material), m_areaLight(areaLight) 
{
	if (m_areaLight != nullptr)
	{
		m_areaLight->setParent(this);
	}
}

bool PrimitiveObject::hit(const Ray& ray) const { return m_shape->hit(ray); }

bool PrimitiveObject::hit(const Ray& ray, SurfaceInteraction& isect) const
{
	Float tHit;
	if (!m_shape->hit(ray, tHit, isect))
		return false;

	ray.m_tMax = tHit;
	isect.primitive = this;
	return true;
}

void PrimitiveObject::computeScatteringFunctions(SurfaceInteraction& isect, MemoryArena& arena,
	TransportMode mode, bool allowMultipleLobes) const
{
	if (m_material != nullptr)
	{
		m_material->computeScatteringFunctions(isect, arena, mode, allowMultipleLobes);
	}
}

Shape* PrimitiveObject::getShape() const { return m_shape.get(); }

Bounds3f PrimitiveObject::worldBound() const { return m_shape->worldBound(); }

const AreaLight* PrimitiveObject::getAreaLight() const { return m_areaLight.get(); }

const Material* PrimitiveObject::getMaterial() const { return m_material; }

// ------------------------ Aggregate ---------------------------------
const AreaLight* PrimitiveAggregate::getAreaLight() const { return nullptr; }

const Material* PrimitiveAggregate::getMaterial() const { return nullptr; }

void PrimitiveAggregate::computeScatteringFunctions(SurfaceInteraction& isect, MemoryArena& arena,
	TransportMode mode, bool allowMultipleLobes) const
{
	//Note: should not go here at all.
	std::cout << "APrimitiveAggregate::computeScatteringFunctions() shouldn't be ""called";
}

RENDER_END