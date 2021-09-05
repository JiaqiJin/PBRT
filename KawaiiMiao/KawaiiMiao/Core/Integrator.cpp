#include "Integrator.h"
#include "../Tool/Parallel.h"
#include "Sampler.h"
#include "Sampling.h"
#include "Scene.h"
#include "../Tool/Memory.h"
#include "../Tool/Reporter.h"
#include "BSDF.h"
#include "LightDistrib.h"

RENDER_BEGIN

void SamplerIntegrator::render(const Scene& scene)
{
	Vector2i resolution = m_camera->m_film->getResolution();

	auto& sampler = m_sampler;

	// Compute number of tiles, _nTiles_, to use for parallel rendering
	Bounds2i sampleBounds = m_camera->m_film->getSampleBounds();
	Vector2i sampleExtent = sampleBounds.diagonal();
	constexpr int tileSize = 16;
	Vector2i nTiles((sampleExtent.x + tileSize - 1) / tileSize, (sampleExtent.y + tileSize - 1) / tileSize);

	Reporter reporter(nTiles.x * nTiles.y, "Rendering");
	AParallelUtils::parallelFor((size_t)0, (size_t)(nTiles.x * nTiles.y), [&](const size_t& t)
	{

			Vector2i tile(t % nTiles.x, t / nTiles.x);
			MemoryArena arena;

			// Get sampler instance for tile
			int seed = t;
			std::unique_ptr<Sampler> tileSampler = sampler->clone(seed);

			// Compute sample bounds for tile
			int x0 = sampleBounds.m_pMin.x + tile.x * tileSize;
			int x1 = glm::min(x0 + tileSize, sampleBounds.m_pMax.x);
			int y0 = sampleBounds.m_pMin.y + tile.y * tileSize;
			int y1 = glm::min(y0 + tileSize, sampleBounds.m_pMax.y);
			Bounds2i tileBounds(Vector2i(x0, y0), Vector2i(x1, y1));
			//K_INFO("Starting image tile");

			// Get _FilmTile_ for tile
			std::unique_ptr<FilmTile> filmTile = m_camera->m_film->getFilmTile(tileBounds);

			// Loop over pixels in tile to render them
			for (Vector2i pixel : tileBounds)
			{
				tileSampler->startPixel(pixel);

				do
				{
					// Initialize _CameraSample_ for current sample
					CameraSample cameraSample = tileSampler->getCameraSample(pixel);

					// Generate camera ray for current sample
					Ray ray;
					Float rayWeight = m_camera->castingRay(cameraSample, ray);

					// Evaluate radiance along camera ray
					Spectrum L(0.f);
					if (rayWeight > 0)
					{
						L = Li(ray, scene, *tileSampler, arena);
					}

					// Issue warning if unexpected radiance value returned
					if (L.hasNaNs())
					{
						K_ERROR(stringPrintf(
							"Not-a-number radiance value returned "
							"for pixel (%d, %d), sample %d. Setting to black.",
							pixel.x, pixel.y,
							(int)tileSampler->currentSampleNumber()));
						L = Spectrum(0.f);
					}
					else if (L.y() < -1e-5)
					{
						K_ERROR(stringPrintf(
							"Negative luminance value, %f, returned "
							"for pixel (%d, %d), sample %d. Setting to black.",
							L.y(), pixel.x, pixel.y,
							(int)tileSampler->currentSampleNumber()));
						L = Spectrum(0.f);
					}
					else if (std::isinf(L.y()))
					{
						K_ERROR(stringPrintf(
							"Infinite luminance value returned "
							"for pixel (%d, %d), sample %d. Setting to black.",
							pixel.x, pixel.y,
							(int)tileSampler->currentSampleNumber()));
						L = Spectrum(0.f);
					}
					//K_INFO("Camera Sample : {0} {1}", cameraSample.pFilm.x, cameraSample.pFilm.y);
					// Add camera ray's contribution to image
					filmTile->addSample(cameraSample.pFilm, L, rayWeight);

					// Free _MemoryArena_ memory from computing image sample value
					arena.Reset();

				} while (tileSampler->startNextSample());
			}
			//K_INFO("Finished image tile {0}", tileBounds.area());

			m_camera->m_film->mergeFilmTile(std::move(filmTile));
			reporter.update();

		}, ExecutionPolicy::PARALLEL);

	reporter.done();

	K_INFO("Rendering finished");

	m_camera->m_film->writeImageToFile();

}

Spectrum SamplerIntegrator::specularReflect(const Ray& ray, const SurfaceInteraction& isect,
	const Scene& scene, Sampler& sampler, MemoryArena& arena, int depth) const
{
	// Compute specular reflection direction _wi_ and BSDF value
	Vector3f wo = isect.wo, wi;
	Float pdf;
	BxDFType type = BxDFType(BSDF_REFLECTION | BSDF_SPECULAR);

	Spectrum f = isect.bsdf->sample_f(wo, wi, sampler.get2D(), pdf, type);

	// Return contribution of specular reflection
	const Vector3f& ns = isect.normal;

	if (pdf > 0.f && !f.isBlack() && absDot(wi, ns) != 0.f)
	{
		// Compute ray differential _rd_ for specular reflection
		Ray rd = isect.spawnRay(wi);
		return f * Li(rd, scene, sampler, arena, depth + 1) * absDot(wi, ns) / pdf;
	}
	else
	{
		return Spectrum(0.f);
	}
}

Spectrum SamplerIntegrator::specularTransmit(const Ray& ray, const SurfaceInteraction& isect,
	const Scene& scene, Sampler& sampler, MemoryArena& arena, int depth) const
{
	Vector3f wo = isect.wo, wi;
	Float pdf;
	const Vector3f& p = isect.p;
	const BSDF& bsdf = *(isect.bsdf);
	BxDFType sampledType = BxDFType(BSDF_TRANSMISSION | BSDF_SPECULAR);
	Spectrum f = bsdf.sample_f(wo, wi, sampler.get2D(), pdf, sampledType);
	Spectrum L = Spectrum(0.f);
	Vector3f ns = isect.normal;

	if (pdf > 0.f && !f.isBlack() && absDot(wi, ns) != 0.f)
	{
		// Compute ray differential _rd_ for specular transmission
		Ray rd = isect.spawnRay(wi);
		L = f * Li(rd, scene, sampler, arena, depth + 1) * absDot(wi, ns) / pdf;
	}
	return L;
}


//------------------------------------------Utility functions-------------------------------------

Spectrum uiformSampleAllLights(const Interaction& it, const Scene& scene,
	MemoryArena& arena, Sampler& sampler, const std::vector<int>& nLightSamples)
{
	Spectrum L(0.f);
	for (size_t j = 0; j < scene.m_lights.size(); ++j)
	{
		// Accumulate contribution of _j_th light to _L_
		const Light::ptr& light = scene.m_lights[j];
		int nSamples = nLightSamples[j];
		const Vector2f* uLightArray = sampler.get2DArray(nSamples);
		const Vector2f* uScatteringArray = sampler.get2DArray(nSamples);

		if (!uLightArray || !uScatteringArray)
		{
			// Use a single sample for illumination from _light_
			Vector2f uLight = sampler.get2D();
			Vector2f uScattering = sampler.get2D();
			L += estimateDirect(it, uScattering, *light, uLight, scene, sampler, arena);
		}
		else
		{
			// Estimate direct lighting using sample arrays
			Spectrum Ld(0.f);
			for (int k = 0; k < nSamples; ++k)
			{
				Ld += estimateDirect(it, uScatteringArray[k], *light, uLightArray[k], scene, sampler, arena);
			}
			L += Ld / nSamples;
		}
	}
	return L;
}

Spectrum uniformSampleOneLight(const Interaction& it, const Scene& scene,
	MemoryArena& arena, Sampler& sampler, const Distribution1D* lightDistrib)
{
	// Randomly choose a single light to sample, _light_
	int nLights = int(scene.m_lights.size());

	if (nLights == 0)
		return Spectrum(0.f);

	int lightNum;
	Float lightPdf;

	if (lightDistrib != nullptr)
	{
		lightNum = lightDistrib->sampleDiscrete(sampler.get1D(), &lightPdf);
		if (lightPdf == 0)
			return Spectrum(0.f);
	}
	else
	{
		lightNum = glm::min((int)(sampler.get1D() * nLights), nLights - 1);
		lightPdf = Float(1) / nLights;
	}

	const Light::ptr& light = scene.m_lights[lightNum];
	Vector2f uLight = sampler.get2D();
	Vector2f uScattering = sampler.get2D();

	return estimateDirect(it, uScattering, *light, uLight, scene, sampler, arena) / lightPdf;
}

Spectrum estimateDirect(const Interaction& it, const Vector2f& uScattering, const Light& light,
	const Vector2f& uLight, const Scene& scene, Sampler& sampler, MemoryArena& arena, bool specular)
{
	BxDFType bsdfFlags = specular ? BSDF_ALL : BxDFType(BSDF_ALL & ~BSDF_SPECULAR);

	Spectrum Ld(0.f);
	// Sample light source with multiple importance sampling
	Vector3f wi;
	Float lightPdf = 0, scatteringPdf = 0;
	VisibilityTester visibility;
	Spectrum Li = light.sample_Li(it, uLight, wi, lightPdf, visibility);

	if (lightPdf > 0 && !Li.isBlack())
	{
		// Compute BSDF or phase function's value for light sample
		Spectrum f;
		// Evaluate BSDF for light sampling strategy
		const SurfaceInteraction& isect = (const SurfaceInteraction&)it;
		f = isect.bsdf->f(isect.wo, wi, bsdfFlags) * absDot(wi, isect.normal);

		scatteringPdf = isect.bsdf->pdf(isect.wo, wi, bsdfFlags);

		if (!f.isBlack())
		{
			// Compute effect of visibility for light source sample
			if (!visibility.unoccluded(scene))
			{
				Li = Spectrum(0.f);
			}

			// Add light's contribution to reflected radiance
			if (!Li.isBlack())
			{
				if (isDeltaLight(light.flags))
				{
					Ld += f * Li / lightPdf;
				}
				else
				{
					Float weight = powerHeuristic(1, lightPdf, 1, scatteringPdf);
					Ld += f * Li * weight / lightPdf;
				}
			}
		}
	}

	// Sample BSDF with multiple importance sampling
	if (!isDeltaLight(light.flags))
	{
		Spectrum f;
		bool sampledSpecular = false;
		// Sample scattered direction for surface interactions
		BxDFType sampledType = BxDFType::BSDF_ALL;
		const SurfaceInteraction& isect = (const SurfaceInteraction&)it;
		f = isect.bsdf->sample_f(isect.wo, wi, uScattering, scatteringPdf, bsdfFlags, sampledType);
		f *= absDot(wi, isect.normal);
		sampledSpecular = (sampledType & BSDF_SPECULAR) != 0;

		if (!f.isBlack() && scatteringPdf > 0)
		{
			// Account for light contributions along sampled direction _wi_
			Float weight = 1;
			if (!sampledSpecular)
			{
				lightPdf = light.pdf_Li(it, wi);
				if (lightPdf == 0) return Ld;
				weight = powerHeuristic(1, scatteringPdf, 1, lightPdf);
			}

			// Find intersection and compute transmittance
			SurfaceInteraction lightIsect;
			Ray ray = it.spawnRay(wi);
			Spectrum Tr(1.f);
			bool foundSurfaceInteraction = scene.hit(ray, lightIsect);

			// Add light contribution from material sampling
			Spectrum Li(0.f);
			if (foundSurfaceInteraction)
			{
				if (lightIsect.primitive->getAreaLight() == &light)
					Li = lightIsect.Le(-wi);
			}
			else
			{
				Li = light.Le(ray);
			}
			if (!Li.isBlack())
				Ld += f * Li * Tr * weight / scatteringPdf;
		}
	}
	return Ld;
}

RENDER_END