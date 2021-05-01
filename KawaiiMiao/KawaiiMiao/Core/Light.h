#pragma once

#include "Rendering.h"
#include "Spectrum.h"
#include "Shape.h"
#include "Interaction.h"
#include "../Math/KMathUtil.h"
#include "../Math/Transform.h"

RENDER_BEGIN

enum class LightFlags : int
{
	LightDeltaPosition = 1,
	LightDeltaDirection = 2,
	LightArea = 4,
	LightInfinite = 8
};

inline bool isDeltaLight(int flags)
{
	return flags & (int)LightFlags::LightDeltaPosition
		|| flags & (int)LightFlags::LightDeltaDirection;
}

class Light
{
public:
	typedef std::shared_ptr<Light> ptr;
	virtual ~Light();

	Light(int flags, const Transform& LightToWorld, int nSamples = 1);

	virtual Spectrum sample_Li(const Interaction& ref, const Vector2f& u,
		Vector3f& wi, Float& pdf, VisibilityTester& vis) const = 0;

	virtual Spectrum power() const = 0;

	virtual void preprocess(const Scene& scene) {}

	virtual Float pdf_Li(const Interaction&, const Vector3f&) const = 0;

	//Emission
	virtual Spectrum Le(const Ray& r) const;

	virtual Spectrum sample_Le(const Vector2f& u1, const Vector2f& u2, Ray& ray,
		Vector3f& nLight, Float& pdfPos, Float& pdfDir) const = 0;

	virtual void pdf_Le(const Ray&, const Vector3f&, Float& pdfPos, Float& pdfDir) const = 0;

	// Light Public Data
	const int flags;
	const int nSamples;
protected:
	// Light Protected Data
	const Transform m_lightToWorld, m_worldToLight;
};

class AreaLight : public Light
{
public:
	typedef std::shared_ptr<AreaLight> ptr;

	AreaLight(const Transform& lightToWorld, int nSamples);
	virtual Spectrum L(const Interaction& intr, const Vector3f& w) const = 0;
};

class DiffuseAreaLight : public AreaLight
{
public:
	typedef std::shared_ptr<DiffuseAreaLight> ptr;

	// DiffuseAreaLight Public Methods
	DiffuseAreaLight(const Transform& lightToWorld, const Spectrum& Le, int nSamples,
		const Shape::ptr& shape, bool twoSided = false);

	virtual Spectrum L(const Interaction& intr, const Vector3f& w) const override
	{
		return dot(intr.normal, w) > 0.f ? m_Lemit : Spectrum(0.f);
	}

	virtual Spectrum power() const override;

	virtual Spectrum sample_Li(const Interaction& ref, const Vector2f& u, Vector3f& wo,
		Float& pdf, VisibilityTester& vis) const override;

	Float pdf_Li(const Interaction& ref, const Vector3f& wi) const;

protected:
	const Spectrum m_Lemit;
	std::shared_ptr<Shape> m_shape;
	const bool m_twoSided;
	const Float m_area;
};

// Visibility Tester
class VisibilityTester final
{
public:
	VisibilityTester() {}
	VisibilityTester(const Interaction& p0, const Interaction& p1)
		: m_p0(p0), m_p1(p1) {}

	const Interaction& P0() const { return m_p0; }
	const Interaction& P1() const { return m_p1; }

	bool unoccluded(const Scene& scene) const;

	Spectrum tr(const Scene& scene, Sampler& sampler) const;

private:
	Interaction m_p0, m_p1;
};


RENDER_END