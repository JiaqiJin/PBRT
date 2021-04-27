#pragma once

#include "Rendering.h"
#include "../Math/KMathUtil.h"
#include "../Math/Transform.h"
#include <vector>

RENDER_BEGIN

class Shape
{
public:
	typedef std::shared_ptr<Shape> ptr;

	Shape(const Transform& objectToWorld, const Transform& worldToObject);
	virtual ~Shape() = default;

	virtual Bounds3f objectBound() const = 0;
	virtual Bounds3f worldBound() const;
	
	virtual bool hit(const Ray& ray) const;
	virtual bool hit(const Ray& ray, Float& tHit, SurfaceInteraction& isect) const = 0;

	virtual Float area() const = 0;

	// Sample a point on the surface of the shape and return the PDF with
	// respect to area on the surface.
	virtual Interaction sample(const Vector2f& u, Float& pdf) const = 0;
	virtual Float pdf(const Interaction&) const { return 1 / area(); }

	// Sample a point on the shape given a reference point |ref| and
	// return the PDF with respect to solid angle from |ref|.
	virtual Interaction sample(const Interaction& ref, const Vector2f& u, Float& pdf) const;
	virtual Float pdf(const Interaction& ref, const Vector3f& wi) const;

	// Returns the solid angle subtended by the shape w.r.t. the reference
	// point p, given in world space. Some shapes compute this value in
	// closed-form, while the default implementation uses Monte Carlo
	// integration; the nSamples parameter determines how many samples are
	// used in this case.
	virtual Float solidAngle(const Vector3f& p, int nSamples = 512) const;

public:
	Transform m_objectToWorld, m_worldToObject;
};

class SphereShape final : public Shape
{
public:
	typedef std::shared_ptr<SphereShape> ptr;

	SphereShape(const Transform& objectToWorld, const Transform& worldToObject, const float radius);

	virtual ~SphereShape() = default;

	virtual Float area() const override;

	virtual Interaction sample(const Vector2f & u, Float & pdf) const override;

	virtual Interaction sample(const Interaction & ref, const Vector2f & u, Float & pdf) const override;
	virtual Float pdf(const Interaction & ref, const Vector3f & wi) const override;

	virtual Bounds3f objectBound() const override;

	virtual bool hit(const Ray & ray) const override;
	virtual bool hit(const Ray & ray, Float & tHit, SurfaceInteraction & isect) const override;

	virtual Float solidAngle(const Vector3f & p, int nSamples = 512) const override;

private:
	Float m_radius;
};

class TriangleShape final : public Shape
{
public:
	typedef std::shared_ptr<TriangleShape> ptr;

	TriangleShape(const Transform& objectToWorld, const Transform& worldToObject, Vector3f v[3]);

	virtual ~TriangleShape() = default;

	virtual Float area() const override;

	virtual Interaction sample(const Vector2f& u, Float& pdf) const override;

	virtual Bounds3f objectBound() const override;
	virtual Bounds3f worldBound() const override;

	virtual bool hit(const Ray& ray) const override;
	virtual bool hit(const Ray& ray, Float& tHit, SurfaceInteraction& isect) const override;

	virtual Float solidAngle(const Vector3f& p, int nSamples = 512) const override;

private:
	Vector3f m_p0, m_p1, m_p2;
};

RENDER_END