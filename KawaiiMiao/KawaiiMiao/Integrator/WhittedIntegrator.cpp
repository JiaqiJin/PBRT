#include "WhittedIntegrator.h"
#include "../Core/Scene.h"
#include "../Core/Interaction.h"
#include "../Core/BSDF.h"

RENDER_BEGIN

// AWhittedIntegrator

WhittedIntegrator::WhittedIntegrator(const APropertyTreeNode& node)
	: SamplerIntegrator(nullptr, nullptr), m_maxDepth(node.getPropertyList().getInteger("Depth", 2))
{
	//Sampler
	const auto& samplerNode = node.getPropertyChild("Sampler");
	m_sampler = Sampler::ptr(static_cast<Sampler*>(AObjectFactory::createInstance(
		samplerNode.getTypeName(), samplerNode)));

	//Camera
	const auto& cameraNode = node.getPropertyChild("Camera");
	m_camera = Camera::ptr(static_cast<Camera*>(AObjectFactory::createInstance(
		cameraNode.getTypeName(), cameraNode)));

	activate();

}

Spectrum WhittedIntegrator::Li(const Ray& ray, const Scene& scene,
	Sampler& sampler, MemoryArena& arena, int depth) const
{
	Spectrum L(0.);

	SurfaceInteraction isect;

	// No intersection found, just return lights emission
	if (!scene.hit(ray, isect))
	{
		for (const auto& light : scene.m_lights)
			L += light->Le(ray);
		return L;
	}

	// Compute emitted and reflected light at ray intersection point

	// Initialize common variables for Whitted integrator
	const Vector3f& n = isect.normal;
	Vector3f wo = isect.wo;

	// Calculate BSDF function for surface interaction
	isect.computeScatteringFunctions(ray, arena);

	// There is no bsdf funcion
	if (!isect.bsdf)
	{
		return Li(isect.spawnRay(ray.direction()), scene, sampler, arena, depth);
	}

	// Compute emitted light if ray hit an area light source -> Le (emission term)
	L += isect.Le(wo);

	// Add contribution of each light source -> shadow ray
	for (const auto& light : scene.m_lights)
	{
		Vector3f wi;
		Float pdf;
		VisibilityTester visibility;
		Spectrum Li = light->sample_Li(isect, sampler.get2D(), wi, pdf, visibility);

		if (Li.isBlack() || pdf == 0)
			continue;

		Spectrum f = isect.bsdf->f(wo, wi);
		if (!f.isBlack() && visibility.unoccluded(scene))
		{
			L += f * Li * absDot(wi, n) / pdf;
		}
	}

	if (depth + 1 < m_maxDepth)
	{
		// Trace rays for specular reflection and refraction
		L += specularReflect(ray, isect, scene, sampler, arena, depth);
		L += specularTransmit(ray, isect, scene, sampler, arena, depth);
	}

	return L;
}


RENDER_END