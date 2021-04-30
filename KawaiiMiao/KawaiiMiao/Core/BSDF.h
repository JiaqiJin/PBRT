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

inline bool sameHemisphere(const Vector3f& w, const Vector3f& wp) { return w.z * wp.z > 0; }

class BSDF
{

};

class BxDF
{
	
};

class Fresnel
{

};

class FresnelNoOp : public Fresnel
{

};

class LambertianReflection : public BxDF
{

};

class SpecularTransmission : public BxDF
{

};

RENDER_END