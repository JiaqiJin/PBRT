#include "Light.h"
#include "Scene.h"
#include "Sampling.h"
#include "../Math/Rng.h"

RENDER_BEGIN

// Light
Light::Light(const APropertyList& props)
{
	nSamples = props.getInteger("LightSamples", 1);
}

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
AreaLight::AreaLight(const APropertyList& props)
: Light(props) 
{ 
	flags = (int)LightFlags::LightArea;
}

AreaLight::AreaLight(const Transform& lightToWorld, int nSamples)
	: Light((int)LightFlags::LightArea, lightToWorld, nSamples) 
{

}

RENDER_END