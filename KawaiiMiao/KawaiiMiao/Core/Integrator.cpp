#include "Integrator.h"
#include "../Tool/Parallel.h"
#include "Sampler.h"
#include "Sampling.h"
#include "Scene.h"
#include "../Tool/Memory.h"
#include "../Tool/Reporter.h"

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
	parallelFor((size_t)0, (size_t)(nTiles.x * nTiles.y), (size_t)1, [&](const tbb::blocked_range<size_t>& range)
		{
			for (size_t t = range.begin(); t != range.end(); ++t)
			{
				Vector2i tile(t % nTiles.x, t / nTiles.x);
				MemoryArena arena;

				// Get sampler instance for tile
				int seed = /*tile.y * nTiles.x + tile.x*/t;
				std::unique_ptr<Sampler> tileSampler = sampler->clone(seed);

				// Compute sample bounds for tile
				int x0 = sampleBounds.m_pMin.x + tile.x * tileSize;
				int x1 = glm::min(x0 + tileSize, sampleBounds.m_pMax.x);
				int y0 = sampleBounds.m_pMin.y + tile.y * tileSize;
				int y1 = glm::min(y0 + tileSize, sampleBounds.m_pMax.y);
				Bounds2i tileBounds(Vector2i(x0, y0), Vector2i(x1, y1));
				K_INFO("Starting image tile " , tileBounds);

				// Get _FilmTile_ for tile
				std::unique_ptr<FilmTile> filmTile = m_camera->m_film->getFilmTile(tileBounds);

				// Loop over pixels in tile to render them
				for (Vector2i pixel : tileBounds)
				{
					tileSampler->startPixel(pixel);

					// Do this check after the StartPixel() call; this keeps
					// the usage of RNG values from (most) Samplers that use
					// RNGs consistent, which improves reproducability /
					// debugging.
					if (!insideExclusive(pixel, m_pixelBounds))
						continue;

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

						K_TRACE("Camera sample: ", cameraSample, " -> ray: ", ray, " -> L = ", L);

						// Add camera ray's contribution to image
						filmTile->addSample(cameraSample.pFilm, L, rayWeight);

						// Free _MemoryArena_ memory from computing image sample value
						arena.Reset();

					} while (tileSampler->startNextSample());
				}
				K_INFO("Finished image tile ",tileBounds);

				m_camera->m_film->mergeFilmTile(std::move(filmTile));
				reporter.update();
			}
		}, ExecutionPolicy::APARALLEL);

	reporter.done();

	K_INFO("Rendering finished");

	m_camera->m_film->writeImageToFile();

}

Spectrum SamplerIntegrator::specularReflect(const Ray& ray, const SurfaceInteraction& isect,
	const Scene& scene, Sampler& sampler, MemoryArena& arena, int depth) const
{
	return Spectrum(1.f);
}

Spectrum SamplerIntegrator::specularTransmit(const Ray& ray, const SurfaceInteraction& isect,
	const Scene& scene, Sampler& sampler, MemoryArena& arena, int depth) const
{
	return Spectrum(1.f);
}

RENDER_END