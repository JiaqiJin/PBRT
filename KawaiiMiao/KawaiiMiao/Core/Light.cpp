#include "Light.h"
#include "Scene.h"
#include "Sampling.h"
#include "../Math/Rng.h"

RENDER_BEGIN
// Light
Light::Light(int flags, const Transform& lightToWorld, int nSamples)
	: flags(flags), nSamples(glm::max(1, nSamples)), m_lightToWorld(lightToWorld),
	m_worldToLight(inverse(lightToWorld))
{
	//++numLights;
}

Light::~Light() {}

Spectrum Light::Le(const Ray& ray) const { return Spectrum(0.f); }
// Visibility tester
bool VisibilityTester::unoccluded(const Scene& scene) const
{
	return !scene.hit(m_p0.spawnRayTo(m_p1));
}

Spectrum VisibilityTester::tr(const Scene& scene, Sampler& sampler) const
{
	//ARay ray(p0.SpawnRayTo(p1));
	Spectrum Tr(1.f);
	//while (true) {
	//	ASurfaceInteraction isect;
	//	bool hitSurface = scene.Intersect(ray, &isect);
	//	// Handle opaque surface along ray's path
	//	if (hitSurface && isect.primitive->GetMaterial() != nullptr)
	//		return ASpectrum(0.0f);

	//	// Update transmittance for current ray segment
	//	if (ray.medium) Tr *= ray.medium->Tr(ray, sampler);

	//	// Generate next ray segment or return final transmittance
	//	if (!hitSurface) break;
	//	ray = isect.SpawnRayTo(p1);
	//}
	return Tr;
}

// Area Light
AreaLight::AreaLight(const Transform& lightToWorld, int nSamples)
	: Light((int)LightFlags::LightArea, lightToWorld, nSamples) 
{

}

// Diffuse Area Light
DiffuseAreaLight::DiffuseAreaLight(const Transform& lightToWorld, const Spectrum& Lemit,
	int nSamples, const Shape::ptr& shape, bool twoSided)
	: AreaLight(lightToWorld, nSamples), m_Lemit(Lemit), m_shape(shape),
	m_twoSided(twoSided), m_area(shape->area())
{ 

}

Spectrum DiffuseAreaLight::power() const
{
	return (m_twoSided ? 2 : 1) * m_Lemit * m_area * Pi;
}

Spectrum DiffuseAreaLight::sample_Li(const Interaction& ref, const Vector2f& u, Vector3f& wi,
	Float& pdf, VisibilityTester& vis) const
{
	Interaction pShape = m_shape->sample(ref, u, pdf);

	if (pdf == 0 || lengthSquared(pShape.p - ref.p) == 0)
	{
		pdf = 0;
		return 0.f;
	}

	wi = normalize(pShape.p - ref.p);
	vis = VisibilityTester(ref, pShape);
	return L(pShape, -wi);
}

Float DiffuseAreaLight::pdf_Li(const Interaction& ref, const Vector3f& wi) const
{
	return m_shape->pdf(ref, wi);
}

Spectrum DiffuseAreaLight::sample_Le(const Vector2f& u1, const Vector2f& u2, Ray& ray,
	Vector3f& nLight, Float& pdfPos, Float& pdfDir) const
{
	// Sample a point on the area light's _Shape_, _pShape_
	Interaction pShape = m_shape->sample(u1, pdfPos);
	nLight = pShape.normal;

	// Sample a cosine-weighted outgoing direction _w_ for area light
	Vector3f w;
	if (m_twoSided)
	{
		Vector2f u = u2;
		// Choose a side to sample and then remap u[0] to [0,1] before
		// applying cosine-weighted hemisphere sampling for the chosen side.
		if (u[0] < .5)
		{
			u[0] = glm::min(u[0] * 2, aOneMinusEpsilon);
			w = cosineSampleHemisphere(u);
		}
		else
		{
			u[0] = glm::min((u[0] - .5f) * 2, aOneMinusEpsilon);
			w = cosineSampleHemisphere(u);
			w.z *= -1;
		}
		pdfDir = 0.5f * cosineHemispherePdf(std::abs(w.z));
	}
	else
	{
		w = cosineSampleHemisphere(u2);
		pdfDir = cosineHemispherePdf(w.z);
	}

	Vector3f v1, v2, n(pShape.normal);
	coordinateSystem(n, v1, v2);
	w = w.x * v1 + w.y * v2 + w.z * n;
	ray = pShape.spawnRay(w);
	return L(pShape, w);
}

void DiffuseAreaLight::pdf_Le(const Ray& ray, const Vector3f& n, Float& pdfPos, Float& pdfDir) const
{
	Interaction it(ray.origin(), n, Vector3f(n));
	pdfPos = m_shape->pdf(it);
	pdfDir = m_twoSided ? (.5 * cosineHemispherePdf(absDot(n, ray.direction())))
		: cosineHemispherePdf(dot(n, ray.direction()));
}

RENDER_END