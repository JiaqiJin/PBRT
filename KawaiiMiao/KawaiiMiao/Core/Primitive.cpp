#include "Primitive.h"

#include "Interaction.h"

RENDER_BEGIN

HitableObject::HitableObject(const Shape::ptr& shape, const Material::ptr& material,
	const AreaLight::ptr& areaLight)
	: m_shape(shape), m_material(material), m_areaLight(areaLight) 
{

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

// Aggregate
const AreaLight* HitableAggregate::getAreaLight() const { return nullptr; }

const Material* HitableAggregate::getMaterial() const { return nullptr; }

void HitableAggregate::computeScatteringFunctions(SurfaceInteraction& isect, MemoryArena& arena,
	TransportMode mode, bool allowMultipleLobes) const
{
	//Note: should not go here at all.
	std::cout <<
		"AHitableAggregate::computeScatteringFunctions() shouldn't be "	"called";
}

Bounds3f HitableList::worldBound() const { return m_worldBounds; }

void HitableList::addHitable(Hitable::ptr entity)
{
	m_hitableList.push_back(entity);
	m_worldBounds = unionBounds(m_worldBounds, entity->worldBound());
}

bool HitableList::hit(const Ray& ray) const
{
	for (int i = 0; i < m_hitableList.size(); i++)
	{
		if (m_hitableList[i]->hit(ray))
		{
			return true;
		}
	}
	return false;
}

bool HitableList::hit(const Ray& ray, SurfaceInteraction& ret) const
{
	SurfaceInteraction temp_rec;
	bool hit_anything = false;
	for (int i = 0; i < m_hitableList.size(); i++)
	{
		if (m_hitableList[i]->hit(ray, temp_rec))
		{
			hit_anything = true;
			ret = temp_rec;
		}
	}
	return hit_anything;
}


RENDER_END