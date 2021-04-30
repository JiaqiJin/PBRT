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

//BSDF
int BSDF::numComponents(BxDFType flags = BSDF_ALL) const
{
	int num = 0;
	for (int i = 0; i < m_nBxDFs; i++)
	{
		if (m_bxdfs[i]->matchesFlags(flags))
			num++;
	}
	return num;
}

Spectrum BSDF::f(const Vector3f& woW, const Vector3f& wiW, BxDFType flags) const
{
	Vector3f wi = worldToLocal(wiW), wo = worldToLocal(woW);
	if (wo.z == 0)
		return 0.f;

	bool reflect = dot(wiW, m_ns) * dot(woW, m_ns) > 0;
	Spectrum f(0.f);
	for (int i = 0; i < m_nBxDFs; i++)
	{
		if (m_bxdfs[i]->matchesFlags(flags) &&
			((reflect && (m_bxdfs[i]->m_type & BSDF_REFLECTION)) ||
				(!reflect && (m_bxdfs[i]->m_type & BSDF_TRANSMISSION))))
		{
			f += m_bxdfs[i]->f(wo, wi);
		}
	}
	return f;
}

Spectrum BSDF::sample_f(const Vector3f& woWorld, Vector3f& wiWorld, const Vector2f& u,
	Float& pdf, BxDFType& sampledType, BxDFType type) const
{
	// Choose which _BxDF_ to sample
	int matchingComps = numComponents(type);
	if (matchingComps == 0)
	{
		pdf = 0;
		if (sampledType)
		{
			sampledType = BxDFType(0);
		}
		return Spectrum(0);
	}
	int comp = glm::min((int)glm::floor(u[0] * matchingComps), matchingComps - 1);

	// Get _BxDF_ pointer for chosen component
	BxDF* bxdf = nullptr;
	int count = comp;
	for (int i = 0; i < m_nBxDFs; ++i)
	{
		if (m_bxdfs[i]->matchesFlags(type) && count-- == 0)
		{
			bxdf = m_bxdfs[i];
			break;
		}
	}
	DCHECK(bxdf != nullptr);

	// Remap _BxDF_ sample _u_ to $[0,1)^2$
	Vector2f uRemapped(glm::min(u[0] * matchingComps - comp, aOneMinusEpsilon), u[1]);

	// Sample chosen _BxDF_
	Vector3f wi, wo = worldToLocal(woWorld);
	if (wo.z == 0)
	{
		return 0.f;
	}

	pdf = 0;
	if (sampledType)
	{
		sampledType = bxdf->m_type;
	}
	Spectrum f = bxdf->sample_f(wo, wi, uRemapped, pdf, sampledType);

	if (pdf == 0)
	{
		if (sampledType)
		{
			sampledType = BxDFType(0);
		}
		return 0;
	}

	wiWorld = localToWorld(wi);

	// Compute overall PDF with all matching _BxDF_s
	if (!(bxdf->m_type & BSDF_SPECULAR) && matchingComps > 1)
	{
		for (int i = 0; i < m_nBxDFs; ++i)
		{
			if (m_bxdfs[i] != bxdf && m_bxdfs[i]->matchesFlags(type))
				pdf += m_bxdfs[i]->pdf(wo, wi);
		}
	}
	if (matchingComps > 1)
	{
		pdf /= matchingComps;
	}

	// Compute value of BSDF for sampled direction
	if (!(bxdf->m_type & BSDF_SPECULAR))
	{
		bool reflect = dot(wiWorld, m_ns) * dot(woWorld, m_ns) > 0;
		f = 0.;
		for (int i = 0; i < m_nBxDFs; ++i)
		{
			if (m_bxdfs[i]->matchesFlags(type) &&
				((reflect && (m_bxdfs[i]->m_type & BSDF_REFLECTION)) ||
					(!reflect && (m_bxdfs[i]->m_type & BSDF_TRANSMISSION))))
			{
				f += m_bxdfs[i]->f(wo, wi);
			}
		}
	}

	return f;
}

Float BSDF::pdf(const Vector3f& woWorld, const Vector3f& wiWorld, BxDFType flags) const
{
	if (m_nBxDFs == 0)
	{
		return 0.f;
	}

	Vector3f wo = worldToLocal(woWorld), wi = worldToLocal(wiWorld);

	if (wo.z == 0)
	{
		return 0.;
	}

	Float pdf = 0.f;
	int matchingComps = 0;
	for (int i = 0; i < m_nBxDFs; ++i)
	{
		if (m_bxdfs[i]->matchesFlags(flags))
		{
			++matchingComps;
			pdf += m_bxdfs[i]->pdf(wo, wi);
		}
	}
	Float v = matchingComps > 0 ? pdf / matchingComps : 0.f;
	return v;
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