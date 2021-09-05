#pragma once

#include "Rendering.h"
#include "Medium.h"
#include "../Math/KMathUtil.h"

RENDER_BEGIN

/**
* Interaction is the point where light interacts with objects
* The structure stores various data
*/
class Interaction
{
public:
	Interaction() = default;
	Interaction(const Vector3f & p) : p(p) {}
	Interaction(const Vector3f & p, const Vector3f & wo) : p(p), wo(normalize(wo)) {}
	Interaction(const Vector3f & p, const Vector3f & n, const Vector3f & wo) : p(p), wo(normalize(wo)), normal(n) {}

	// New Constructor
	Interaction(const Vector3f& p, const Vector3f& n, const Vector3f& pError,
		const Vector3f& wo, Float time,
		const MediumInterface& mediumInterface)
		: p(p),
		time(time),
		pError(pError),
		wo(normalize(wo)),
		normal(n),
		mediumInterface(mediumInterface) {}

	Interaction(const Vector3f& p, const Vector3f& wo, Float time,
		const MediumInterface& mediumInterface)
		: p(p), time(time), wo(wo), mediumInterface(mediumInterface) {}
	Interaction(const Vector3f& p, Float time,
		const MediumInterface& mediumInterface)
		: p(p), time(time), mediumInterface(mediumInterface) {}

	inline Ray spawnRay(const Vector3f& dir) const
	{
		Vector3f origin = p;
		return Ray(origin, dir, Infinity);
	}

	inline Ray spawnRayTo(const Vector3f& p2) const
	{
		Vector3f origin = p;
		Vector3f dir = p2 - origin;
		return Ray(origin, dir, 1 - ShadowEpsilon);
	}

	inline Ray spawnRayTo(const Interaction& it) const
	{
		Vector3f origin = p;
		Vector3f target = it.p;
		Vector3f d = target - origin;
		return Ray(origin, d, 1 - ShadowEpsilon);
	}

public:
	Vector3f p; // surface point
	Vector3f wo; // outgoing dir
	Vector3f normal; // normal
	Vector3f pError;
	Float time;
	MediumInterface mediumInterface;
};

class SurfaceInteraction final : public Interaction
{
public:
	SurfaceInteraction() = default;
	SurfaceInteraction(const Vector3f& p, const Vector2f& uv, const Vector3f& wo,
		const Vector3f& dpdu, const Vector3f& dpdv, const Shape* shape);

	SurfaceInteraction(const Vector3f& p, const Vector3f& pError,
		const Vector2f& uv, const Vector3f& wo,
		const Vector3f& dpdu, const Vector3f& dpdv,
		const Vector3f& dndu, const Vector3f& dndv, Float time,
		const Shape* sh,
		int faceIndex = 0);

	Spectrum Le(const Vector3f& w) const;

	void computeScatteringFunctions(const Ray& ray, MemoryArena& arena,
		bool allowMultipleLobes = false, TransportMode mode = TransportMode::Radiance);

public:
	Vector2f uv;
	Vector3f dpdu, dpdv;
	Vector3f dndu, dndv;
	BSDF* bsdf = nullptr;

	const Shape* shape = nullptr;
	const Primitive* primitive = nullptr;

	struct 
	{
		Vector3f n;
		Vector3f dpdu, dpdv;
		Vector3f dndu, dndv;
	} shading;

	int faceIndex = 0;
	mutable Vector3f dpdx, dpdy;
	mutable Float dudx = 0, dvdx = 0, dudy = 0, dvdy = 0;
};

class MediumInteraction : public Interaction
{
public:
	MediumInteraction() : phase(nullptr) {}

	bool isValid() const 
	{
		return phase != nullptr;
	}
	const PhaseFunction* phase;
};

RENDER_END