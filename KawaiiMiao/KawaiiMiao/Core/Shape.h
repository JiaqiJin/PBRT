#pragma once

#include "Rendering.h"
#include "Rtti.h"
#include "../Math/KMathUtil.h"
#include "../Math/Transform.h"
#include <vector>

RENDER_BEGIN

/*
The Shape base class defines the general Shape interface. 
*/
class Shape : public AObject
{
public:
	typedef std::shared_ptr<Shape> ptr;

	Shape(const APropertyList& props);
	Shape(Transform* objectToWorld, Transform* worldToObject);
	virtual ~Shape() = default;

	void setTransform(Transform* objectToWorld, Transform* worldToObject);

	// Return the bounding box in the object coordinate system
	virtual Bounds3f objectBound() const = 0;

	// Return the bounding box in the world coordinate system
	virtual Bounds3f worldBound() const;

	virtual bool hit(const Ray& ray) const;
	// Intersection function, fill in SurfaceInteraction data
	// Almost all calculations for the intersection of shape and ray are performed by converting ray into object space
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

	virtual ClassType getClassType() const override { return ClassType::RShape; }

public:
	Transform* m_objectToWorld = nullptr; 
	Transform* m_worldToObject = nullptr;
};

RENDER_END