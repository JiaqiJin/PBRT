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

RENDER_END