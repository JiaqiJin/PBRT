#pragma once

#include "Rendering.h"
#include "Light.h"
#include "Shape.h"
#include "Material.h"
#include "Rtti.h"

RENDER_BEGIN

class Primitive : public AObject
{
public:
	typedef std::shared_ptr<Primitive> ptr;

	virtual ~Primitive() = default;

	virtual bool hit(const Ray & ray) const = 0;
	virtual bool hit(const Ray & ray, SurfaceInteraction & iset) const = 0;

	virtual Bounds3f worldBound() const = 0;

	virtual const AreaLight* getAreaLight() const = 0;
	virtual const Material* getMaterial() const = 0;

	virtual void computeScatteringFunctions(SurfaceInteraction & isect, MemoryArena & arena,
		TransportMode mode, bool allowMultipleLobes) const = 0;

	virtual ClassType getClassType() const override { return ClassType::RPrimitive; }
};

class PrimitiveObject : public Primitive
{
public:
	typedef std::shared_ptr<PrimitiveObject> ptr;

	PrimitiveObject(const Shape::ptr &shape, const Material* material, 
		const AreaLight::ptr& areaLight);
	virtual Bounds3f worldBound() const;
	virtual bool hit(const Ray& ray) const override;
	virtual bool hit(const Ray& ray, SurfaceInteraction& iset) const override;

	Shape* getShape() const;

	AreaLight::ptr getAreaLightPtr() const { return m_areaLight; }
	virtual const AreaLight* getAreaLight() const override;
	virtual const Material* getMaterial() const override;

	virtual void computeScatteringFunctions(SurfaceInteraction& isect, MemoryArena& arena,
		TransportMode mode, bool allowMultipleLobes) const override;

	virtual std::string toString() const override { return "PrimitiveObject[]"; }

private:
	Shape::ptr m_shape;
	AreaLight::ptr m_areaLight;
	const Material* m_material;
};

class PrimitiveAggregate : public Primitive
{
public:
	virtual const AreaLight* getAreaLight() const override;
	virtual const Material* getMaterial() const override;

	virtual void computeScatteringFunctions(SurfaceInteraction& isect, MemoryArena& arena,
		TransportMode mode, bool allowMultipleLobes) const override;
};

RENDER_END