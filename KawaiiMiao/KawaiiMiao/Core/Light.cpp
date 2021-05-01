#include "Light.h"
#include "Scene.h"

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
	:Light((int)LightFlags::LightArea, lightToWorld, nSamples)
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

RENDER_END