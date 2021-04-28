#include "Sampling.h"

RENDER_BEGIN

Vector3f uniformSampleHemisphere(const Vector2f& u)
{
	Float z = u[0];
	Float r = glm::sqrt(glm::max((Float)0, (Float)1. - z * z));
	Float phi = 2 * Pi * u[1];
	return Vector3f(r * glm::cos(phi), r * glm::sin(phi), z);
}

Float uniformHemispherePdf() { return Inv2Pi; }

Vector3f uniformSampleSphere(const Vector2f& u)
{
	Float z = 1 - 2 * u[0];
	Float r = glm::sqrt(glm::max((Float)0, (Float)1 - z * z));
	Float phi = 2 * Pi * u[1];
	return Vector3f(r * glm::cos(phi), r * glm::sin(phi), z);
}

Float uniformSpherePdf() { return Inv4Pi; }

Vector3f uniformSampleCone(const Vector2f& u, Float cosThetaMax)
{
	Float cosTheta = ((Float)1 - u[0]) + u[0] * cosThetaMax;
	Float sinTheta = glm::sqrt((Float)1 - cosTheta * cosTheta);
	Float phi = u[1] * 2 * Pi;
	return Vector3f(glm::cos(phi) * sinTheta, glm::sin(phi) * sinTheta, cosTheta);
}

Vector3f uniformSampleCone(const Vector2f& u, Float cosThetaMax, const Vector3f& x,
	const Vector3f& y, const Vector3f& z)
{
	Float cosTheta = lerp(u[0], cosThetaMax, 1.f);
	Float sinTheta = glm::sqrt((Float)1. - cosTheta * cosTheta);
	Float phi = u[1] * 2 * Pi;
	return glm::cos(phi) * sinTheta * x + glm::sin(phi) * sinTheta * y + cosTheta * z;
}

Float uniformConePdf(Float cosThetaMax) { return 1 / (2 * Pi * (1 - cosThetaMax)); }

Vector2f concentricSampleDisk(const Vector2f& u)
{
	// Map uniform random numbers to $[-1,1]^2$
	Vector2f uOffset = 2.f * u - Vector2f(1, 1);

	// Handle degeneracy at the origin
	if (uOffset.x == 0 && uOffset.y == 0)
		return Vector2f(0, 0);

	// Apply concentric mapping to point
	Float theta, r;
	if (glm::abs(uOffset.x) > glm::abs(uOffset.y))
	{
		r = uOffset.x;
		theta = PiOver4 * (uOffset.y / uOffset.x);
	}
	else
	{
		r = uOffset.y;
		theta = PiOver2 - PiOver4 * (uOffset.x / uOffset.y);
	}
	return r * Vector2f(glm::cos(theta), glm::sin(theta));
}

Vector2f uniformSampleTriangle(const Vector2f& u)
{
	Float su0 = glm::sqrt(u[0]);
	return Vector2f(1 - su0, u[1] * su0);
}

RENDER_END