#pragma once

#include "../Core/Light.h"

RENDER_BEGIN

class DiffuseAreaLight final : public AreaLight
{
public:
	typedef std::shared_ptr<DiffuseAreaLight> ptr;

	DiffuseAreaLight(const APropertyTreeNode& node);

	DiffuseAreaLight(const Transform& lightToWorld, const Spectrum& Le, int nSamples,
		Shape* shape, bool twoSided = false);

	virtual Spectrum L(const Interaction& intr, const Vector3f& w) const override
	{
		return (m_twoSided || dot(intr.normal, w) > 0) ? m_Lemit : Spectrum(0.f);
	}

	virtual Spectrum power() const override;

	virtual Spectrum sample_Li(const Interaction& ref, const Vector2f& u, Vector3f& wo,
		Float& pdf, VisibilityTester& vis) const override;

	virtual Float pdf_Li(const Interaction&, const Vector3f&) const override;

	virtual Spectrum sample_Le(const Vector2f& u1, const Vector2f& u2, Ray& ray,
		Vector3f& nLight, Float& pdfPos, Float& pdfDir) const override;

	virtual void pdf_Le(const Ray&, const Vector3f&, Float& pdfPos, Float& pdfDir) const override;

	virtual std::string toString() const override { return "DiffuseAreaLight[]"; }

	virtual void setParent(AObject* parent) override;

protected:
	Spectrum m_Lemit;
	Shape* m_shape;
	// Added after book publication: by default, DiffuseAreaLights still
	// only emit in the hemimsphere around the surface normal.  However,
	// this behavior can now be overridden to give emission on both sides.
	bool m_twoSided;
	Float m_area;
};

RENDER_END