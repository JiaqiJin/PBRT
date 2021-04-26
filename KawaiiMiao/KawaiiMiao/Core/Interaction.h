#pragma once

#include "Rendering.h"
#include "../Math/KMathUtil.h"

RENDER_BEGIN

class Interaction
{
public:
	Interaction() = default;
	Interaction(const Vector3f & p) : p(p) {}
	Interaction(const Vector3f & p, const Vector3f & wo) : p(p), wo(normalize(wo)) {}
	Interaction(const Vector3f & p, const Vector3f & n, const Vector3f & wo)
		: p(p), wo(normalize(wo)), normal(n) {}

	inline Ray spawnRay(const Vector3f& dir)
	{
		Vector3f origin = p;
		return Ray(origin, dir, Infinity);
	}

	inline Ray spawnRayTo(const Vector3f& p2)
	{
		Vector3f origin = p;
		Vector3f dir = p2 - origin;
		return Ray(origin, dir, 1 - ShadowEpsilon);
	}

	inline Ray spawnRayTo(const Interaction& it)
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
};

class SurfaceInteraction final : public Interaction
{
public:
	SurfaceInteraction() = default;
	SurfaceInteraction(const Vector3f& p, const Vector2f& uv, const Vector3f& wo,
		const Vector3f& dpdu, const Vector3f& dpdv, const Shape* shape);

	Spectrum Le(const Vector3f& w) const;

	void computeScatteringFunctions(const Ray& ray, MemoryArena& arena,
		bool allowMultipleLobes = false, TransportMode mode = TransportMode::Radiance);

public:
	Vector2f uv;
	Vector3f dpdu, dpdv;

	BSDF* bsdf = nullptr;

	const Shape* shape = nullptr;
	const Hitable* hitable = nullptr;
};

RENDER_END