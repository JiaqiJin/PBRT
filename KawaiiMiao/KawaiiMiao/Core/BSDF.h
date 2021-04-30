#pragma once

#include "Rendering.h"
#include "Interaction.h"
#include "../Math/KMathUtil.h"
#include "Spectrum.h"

RENDER_BEGIN

enum BxDFType
{
	BSDF_REFLECTION = 1 << 0,
	BSDF_TRANSMISSION = 1 << 1,
	BSDF_DIFFUSE = 1 << 2,
	BSDF_GLOSSY = 1 << 3,
	BSDF_SPECULAR = 1 << 4,
	BSDF_ALL = BSDF_DIFFUSE | BSDF_GLOSSY | BSDF_SPECULAR | BSDF_REFLECTION | BSDF_TRANSMISSION,
};

Float frDielectric(Float cosThetaI, Float etaI, Float etaT);
Spectrum FrConductor(Float cosThetaI, const Spectrum& etaI, const Spectrum& etaT, const Spectrum& k);

inline bool sameHemisphere(const Vector3f& w, const Vector3f& wp) { return w.z * wp.z > 0; }

class BSDF
{

};

class BxDF
{
public:
	BxDF(BxDFType type) : m_type(type) {}

	virtual ~BxDF() = default;

	bool matchesFlags(BxDFType t) const { return (m_type & t) == m_type; }

	virtual Spectrum f(const Vector3f& wo, const Vector3f& wi) const = 0;
	virtual Spectrum sample_f(const Vector3f& wo, Vector3f& wi, const Vector2f& sample,
		Float& pdf, BxDFType& sampledType) const;

	virtual Float pdf(const Vector3f& wo, const Vector3f& wi) const;

	// BxDF Public Data
	const BxDFType m_type;
};

// Fresnel
class Fresnel
{
public:
	virtual ~Fresnel() = default;
	virtual Spectrum evaluate(Float cosI) const = 0;
};

class FresnelDielectric : public Fresnel
{
public:
	FresnelDielectric(Float etaI, Float etaT) : m_etaI(etaI), m_etaT(etaT) {}

	virtual Spectrum evaluate(Float cosThetaI) const override
	{
		return frDielectric(cosThetaI, m_etaI, m_etaT);
	}

private:
	Float m_etaI, m_etaT;
};

class FresnelConductor : public Fresnel {
public:
	FresnelConductor(const Spectrum& etaI, const Spectrum& etaT, const Spectrum& k)
		: m_etaI(etaI), m_etaT(etaT), k(k) { }

	virtual Spectrum evaluate(Float cosThetaI) const override
	{
		return FrConductor(std::abs(cosThetaI), m_etaI, m_etaT, k);
	}
private:
	Spectrum m_etaI, m_etaT, k;
};

class FresnelNoOp : public Fresnel
{
public:
	virtual Spectrum evaluate(Float) const override { return Spectrum(1.); }
};

// Reflections
class LambertianReflection : public BxDF
{
public:
	// LambertianReflection Public Methods
	LambertianReflection(const Spectrum& R)
		: BxDF(BxDFType(BSDF_REFLECTION | BSDF_DIFFUSE)), m_R(R) {}

	virtual Spectrum f(const Vector3f& wo, const Vector3f& wi) const override;

private:
	// LambertianReflection Private Data
	const Spectrum m_R;
};

class SpecularReflection : public BxDF
{
public:
	SpecularReflection(const Spectrum& R, Fresnel* fresnel)
		: BxDF(BxDFType(BSDF_REFLECTION | BSDF_SPECULAR)),
		m_R(R), m_Fresnel(fresnel) {}

	virtual Spectrum f(const Vector3f& wo, const Vector3f& wi) const override { return Spectrum(0.f); }

	virtual Spectrum sample_f(const Vector3f& wo, Vector3f& wi, const Vector2f& sample,
		Float& pdf, BxDFType& sampledType) const override;

	virtual Float pdf(const Vector3f& wo, const Vector3f& wi) const override { return 0.f; }

private:
	const Spectrum m_R;
	const Fresnel* m_Fresnel;
};

// Transmitions
class SpecularTransmission : public BxDF
{
public:
	// SpecularTransmission Public Methods
	SpecularTransmission(const Spectrum& T, Float etaA, Float etaB, TransportMode mode)
		: BxDF(BxDFType(BSDF_TRANSMISSION | BSDF_SPECULAR)), m_T(T), m_etaA(etaA),
		m_etaB(etaB), m_fresnel(etaA, etaB), m_mode(mode) {}

	virtual Spectrum f(const Vector3f& wo, const Vector3f& wi) const override { return Spectrum(0.f); }

	virtual Spectrum sample_f(const Vector3f& wo, Vector3f& wi, const Vector2f& sample,
		Float& pdf, BxDFType& sampledType) const override;

	virtual Float pdf(const Vector3f& wo, const Vector3f& wi) const override { return 0.f; }

private:
	const Spectrum m_T;
	const Float m_etaA, m_etaB;
	const FresnelDielectric m_fresnel;
	const TransportMode m_mode;
};

RENDER_END