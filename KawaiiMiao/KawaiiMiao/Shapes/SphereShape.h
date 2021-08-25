#pragma once

#include "../Core/Shape.h"

RENDER_BEGIN

class SphereShape final : public Shape
{
public:
	typedef std::shared_ptr<SphereShape> ptr;

	SphereShape(const APropertyTreeNode& node);
	SphereShape(Transform* objectToWorld, Transform* worldToObject, const float radius);

	virtual ~SphereShape() = default;

	virtual Float area() const override;

	virtual Interaction sample(const Vector2f& u, Float& pdf) const override;

	virtual Interaction sample(const Interaction& ref, const Vector2f& u, Float& pdf) const override;
	virtual Float pdf(const Interaction& ref, const Vector3f& wi) const override;

	virtual Bounds3f objectBound() const override;

	virtual bool hit(const Ray& ray) const override;
	virtual bool hit(const Ray& ray, Float& tHit, SurfaceInteraction& isect) const override;

	virtual Float solidAngle(const Vector3f& p, int nSamples = 512) const override;

	virtual std::string toString() const override { return "SphereShape[]"; }

private:
	Float m_radius;
};


RENDER_END