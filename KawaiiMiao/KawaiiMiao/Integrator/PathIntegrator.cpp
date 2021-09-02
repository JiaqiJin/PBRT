#include "PathIntegrator.h"

#include "../Core/BSDF.h"
#include "../Core/Scene.h"

RENDER_BEGIN

RENDER_REGISTER_CLASS(PathIntegrator, "Path")

PathIntegrator::PathIntegrator(const APropertyTreeNode& node)
	: SamplerIntegrator(nullptr, nullptr), m_maxDepth(node.getPropertyList().getInteger("Depth", 2))
	, m_rrThreshold(1.f), m_lightSampleStrategy("spatial")
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

PathIntegrator::PathIntegrator(int maxDepth, Camera::ptr camera, Sampler::ptr sampler,
	Float rrThreshold, const std::string& lightSampleStrategy)
	: SamplerIntegrator(camera, sampler), m_maxDepth(maxDepth),
	m_rrThreshold(rrThreshold), m_lightSampleStrategy(lightSampleStrategy) {}

void PathIntegrator::preprocess(const Scene& scene)
{
	m_lightDistribution = createLightSampleDistribution(m_lightSampleStrategy, scene);
}

Spectrum PathIntegrator::Li(const Ray& r, const Scene& scene, Sampler& sampler,
	MemoryArena& arena, int depth) const
{
	Spectrum L(0.f), beta(1.f);
	Ray ray(r);

	bool specularBounce = false;
	int bounces;
	// Added after book publication: etaScale tracks the accumulated effect
	// of radiance scaling due to rays passing through refractive
	// boundaries (see the derivation on p. 527 of the third edition). We
	// track this value in order to remove it from beta when we apply
	// Russian roulette; this is worthwhile, since it lets us sometimes
	// avoid terminating refracted rays that are about to be refracted back
	// out of a medium and thus have their beta value increased.
	Float etaScale = 1;

	for (bounces = 0;; ++bounces)
	{
		// Find next path vertex and accumulate contribution

		// Intersect _ray_ with scene and store intersection in _isect_
		SurfaceInteraction isect;
		bool hit = scene.hit(ray, isect);

		// Possibly add emitted light at intersection
		if (bounces == 0 || specularBounce)
		{
			// Add emitted light at path vertex or from the environment
			if (hit)
			{
				L += beta * isect.Le(-ray.direction());
			}
			else
			{
				for (const auto& light : scene.m_infiniteLights)
					L += beta * light->Le(ray);
			}
		}

		// Terminate path if ray escaped or _maxDepth_ was reached
		if (!hit || bounces >= m_maxDepth)
			break;

		// Compute scattering functions and skip over medium boundaries
		isect.computeScatteringFunctions(ray, arena, true);

		// Note: bsdf == nullptr indicates that the current surface has no effect on light,
		//       and such surfaces are used to represent transitions between participating 
		//		 media, whose boundaries are themselves optically inactive.
		if (!isect.bsdf)
		{
			ray = isect.spawnRay(ray.direction());
			bounces--;
			continue;
		}

		const Distribution1D* distrib = m_lightDistribution->lookup(isect.p);

		// Sample illumination from lights to find path contribution.
		// (But skip this for perfectly specular BSDFs.)
		if (isect.bsdf->numComponents(BxDFType(BSDF_ALL & ~BSDF_SPECULAR)) > 0)
		{
			//++totalPaths;
			Spectrum Ld = beta * uniformSampleOneLight(isect, scene, arena, sampler, distrib);
			//if (Ld.isBlack()) 
			//	++zeroRadiancePaths;
			CHECK_GE(Ld.y(), 0.f);
			L += Ld;
		}

		// Sample BSDF to get new path direction
		Vector3f wo = -ray.direction(), wi;
		Float pdf;
		BxDFType flags;
		Spectrum f = isect.bsdf->sample_f(wo, wi, sampler.get2D(), pdf, flags, BSDF_ALL);

		if (f.isBlack() || pdf == 0.f)
			break;
		beta *= f * absDot(wi, isect.normal) / pdf;

		CHECK_GE(beta.y(), 0.f);
		DCHECK(!glm::isinf(beta.y()));

		specularBounce = (flags & BSDF_SPECULAR) != 0;
		if ((flags & BSDF_SPECULAR) && (flags & BSDF_TRANSMISSION))
		{
			Float eta = isect.bsdf->m_eta;
			// Update the term that tracks radiance scaling for refraction
			// depending on whether the ray is entering or leaving the
			// medium.
			etaScale *= (dot(wo, isect.normal) > 0) ? (eta * eta) : 1 / (eta * eta);
		}

		ray = isect.spawnRay(wi);

		// Possibly terminate the path with Russian roulette.
		// Factor out radiance scaling due to refraction in rrBeta.
		Spectrum rrBeta = beta * etaScale;
		if (rrBeta.maxComponentValue() < m_rrThreshold && bounces > 3)
		{
			Float q = glm::max((Float).05f, 1 - rrBeta.maxComponentValue());
			if (sampler.get1D() < q)
				break;
			beta /= 1 - q;
			DCHECK(!glm::isinf(beta.y()));
		}
	}

	//ReportValue(pathLength, bounces);
	return L;
}


RENDER_END