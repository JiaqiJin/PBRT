#include "BSDF.h"

#include "Sampler.h"
#include "Sampling.h"

RENDER_BEGIN

Float frDielectric(Float cosThetaI, Float etaI, Float etaT)
{
	cosThetaI = clamp(cosThetaI, -1, 1);
	// Potentially swap indices of refraction
	bool entering = cosThetaI > 0.f;
	if (!entering)
	{
		std::swap(etaI, etaT);
		cosThetaI = glm::abs(cosThetaI);
	}

	// Compute _cosThetaT_ using Snell's law
	Float sinThetaI = glm::sqrt(glm::max((Float)0, 1 - cosThetaI * cosThetaI));
	Float sinThetaT = etaI / etaT * sinThetaI;

	// Handle total internal reflection
	if (sinThetaT >= 1)
		return 1;
	Float cosThetaT = glm::sqrt(glm::max((Float)0, 1 - sinThetaT * sinThetaT));
	Float Rparl = ((etaT * cosThetaI) - (etaI * cosThetaT)) / ((etaT * cosThetaI) + (etaI * cosThetaT));
	Float Rperp = ((etaI * cosThetaI) - (etaT * cosThetaT)) / ((etaI * cosThetaI) + (etaT * cosThetaT));
	return (Rparl * Rparl + Rperp * Rperp) / 2;
}

// https://seblagarde.wordpress.com/2013/04/29/memo-on-fresnel-equations/
Spectrum FrConductor(Float cosThetaI, const Spectrum& etaI, const Spectrum& etaT, const Spectrum& k)
{
	cosThetaI = clamp(cosThetaI, -1, 1);
	Spectrum eta = etaT / etaI;
	Spectrum etak = k / etaI;

	Float cosThetaI2 = cosThetaI * cosThetaI;
	Float sinThetaI2 = 1. - cosThetaI2;
	Spectrum eta2 = eta * eta;
	Spectrum etak2 = etak * etak;

	Spectrum t0 = eta2 - etak2 - sinThetaI2;
	Spectrum a2plusb2 = sqrt(t0 * t0 + 4 * eta2 * etak2);
	Spectrum t1 = a2plusb2 + cosThetaI2;
	Spectrum a = sqrt(0.5f * (a2plusb2 + t0));
	Spectrum t2 = (Float)2 * cosThetaI * a;
	Spectrum Rs = (t1 - t2) / (t1 + t2);

	Spectrum t3 = cosThetaI2 * a2plusb2 + sinThetaI2 * sinThetaI2;
	Spectrum t4 = t2 * sinThetaI2;
	Spectrum Rp = Rs * (t3 - t4) / (t3 + t4);

	return 0.5 * (Rp + Rs);
}

// BxDF

Spectrum BxDF::sample_f(const Vector3f& wo, Vector3f& wi, const Vector2f& sample,
	Float& pdf, BxDFType& sampledType) const
{
	// Cosine-sample the hemisphere, flipping the direction if necessary
	wi = cosineSampleHemisphere(sample);
	if (wo.z < 0)
	{
		wi.z *= -1;
	}

	pdf = this->pdf(wo, wi);

	return f(wo, wi);
}

Float BxDF::pdf(const Vector3f& wo, const Vector3f& wi) const
{
	return sameHemisphere(wo, wi) ? glm::abs(wi.z) * InvPi : 0;
}

// Lambertian
Spectrum LambertianReflection::f(const Vector3f& wo, const Vector3f& wi) const
{
	return m_R * InvPi;
}

// SpecularReflection 
Spectrum SpecularReflection::sample_f(const Vector3f& wo, Vector3f& wi, const Vector2f& sample,
	Float& pdf, BxDFType& sampledType) const
{
	wi = Vector3f(-wo.x, -wo.y, wo.z);
	pdf = 1;
	return m_Fresnel->evaluate(wi.z) * m_R / glm::abs(wi.z);
}

// SpecularTransmition
Spectrum SpecularTransmission::sample_f(const Vector3f& wo, Vector3f& wi, const Vector2f& sample,
	Float& pdf, BxDFType& sampledType) const
{
	bool entering = (wo.z) > 0;
	Float etaI = entering ? m_etaA : m_etaB;
	Float etaT = entering ? m_etaB : m_etaA;

	if (!refract(wo, faceforward(Vector3f(0, 0, 1), wo), etaI / etaT, wi))
		return 0;

	pdf = 1;
	Spectrum ft = m_T * (Spectrum(1.) - m_fresnel.evaluate(wi.z));
	// Account for non-symmetry with transmission to different medium
	if (m_mode == TransportMode::Radiance)
		ft *= (etaI * etaI) / (etaT * etaT);
	return ft / glm::abs(wi.z);
}

RENDER_END