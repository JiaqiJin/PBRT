#pragma once

#include "Rendering.h"
#include "Light.h"
#include "Shape.h"
#include "Material.h"
#include "Rtti.h"

RENDER_BEGIN

class Hitable : public AObject
{
public:
	typedef std::shared_ptr<Hitable> ptr;

	virtual ~Hitable() = default;

	virtual bool hit(const Ray & ray) const = 0;
	virtual bool hit(const Ray & ray, SurfaceInteraction & iset) const = 0;

	virtual Bounds3f worldBound() const = 0;

	virtual const AreaLight* getAreaLight() const = 0;
	virtual const Material* getMaterial() const = 0;

	virtual void computeScatteringFunctions(SurfaceInteraction & isect, MemoryArena & arena,
		TransportMode mode, bool allowMultipleLobes) const = 0;

	virtual ClassType getClassType() const override { return ClassType::RHitable; }
};

class HitableObject : public Hitable
{
public:
	typedef std::shared_ptr<HitableObject> ptr;

	HitableObject(const Shape::ptr &shape, const Material::ptr& material,
		const AreaLight::ptr& areaLight);
	virtual Bounds3f worldBound() const;
	virtual bool hit(const Ray& ray) const override;
	virtual bool hit(const Ray& ray, SurfaceInteraction& iset) const override;

	AreaLight::ptr getAreaLightPtr() const { return m_areaLight; }
	virtual const AreaLight* getAreaLight() const override;
	virtual const Material* getMaterial() const override;

	virtual void computeScatteringFunctions(SurfaceInteraction& isect, MemoryArena& arena,
		TransportMode mode, bool allowMultipleLobes) const override;

	virtual std::string toString() const override { return "HitableObject[]"; }

private:
	Shape::ptr m_shape;
	AreaLight::ptr m_areaLight;
	Material::ptr m_material;
};

class HitableAggregate : public Hitable
{
public:
	virtual const AreaLight* getAreaLight() const override;
	virtual const Material* getMaterial() const override;

	virtual void computeScatteringFunctions(SurfaceInteraction& isect, MemoryArena& arena,
		TransportMode mode, bool allowMultipleLobes) const override;
};

class HitableList final : public HitableAggregate
{
public:
	typedef std::shared_ptr<HitableList> ptr;

	HitableList() = default;
	virtual bool hit(const Ray & ray) const override;
	virtual bool hit(const Ray & ray, SurfaceInteraction & iset) const override;

	virtual Bounds3f worldBound() const override;

	bool isEmpty() const { return m_hitableList.empty(); }
	void addHitable(Hitable::ptr entity);

private:
	using AHitableBuffer = std::vector<Hitable::ptr>;
	AHitableBuffer m_hitableList;
	Bounds3f m_worldBounds;
};

RENDER_END