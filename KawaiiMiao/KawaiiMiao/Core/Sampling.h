#pragma once

#include "Rendering.h"
#include "../Math/KMathUtil.h"

RENDER_BEGIN

Vector3f uniformSampleHemisphere(const Vector2f& u);

Float uniformHemispherePdf();

Vector3f uniformSampleSphere(const Vector2f& u);

Float uniformSpherePdf();

Vector3f uniformSampleCone(const Vector2f& u, Float thetamax);

Vector3f uniformSampleCone(const Vector2f& u, Float thetamax, const Vector3f& x,
	const Vector3f& y, const Vector3f& z);

Float uniformConePdf(Float thetamax);

Vector2f concentricSampleDisk(const Vector2f& u);

Vector2f uniformSampleTriangle(const Vector2f& u);

inline Vector3f cosineSampleHemisphere(const Vector2f& u)
{
	Vector2f d = concentricSampleDisk(u);
	Float z = std::sqrt(glm::max((Float)0, 1 - d.x * d.x - d.y * d.y));
	return Vector3f(d.x, d.y, z);
}

inline Float cosineHemispherePdf(Float cosTheta) { return cosTheta * InvPi; }

inline Float balanceHeuristic(int nf, Float fPdf, int ng, Float gPdf)
{
	return (nf * fPdf) / (nf * fPdf + ng * gPdf);
}

inline Float powerHeuristic(int nf, Float fPdf, int ng, Float gPdf)
{
	Float f = nf * fPdf, g = ng * gPdf;
	return (f * f) / (f * f + g * g);
}

RENDER_END