#pragma once

#include "../Core/Integrator.h"

RENDER_BEGIN

class WhittedIntegrator : public SamplerIntegrator
{
public:
	// WhittedIntegrator Public Methods
	WhittedIntegrator(int maxDepth, Camera::ptr camera, Sampler::ptr sampler,
		const Bounds2i& pixelBounds)
		: SamplerIntegrator(camera, sampler), m_maxDepth(maxDepth) {}

	virtual Spectrum Li(const Ray& ray, const Scene& scene,
		Sampler& sampler, MemoryArena& arena, int depth) const override;

private:
	const int m_maxDepth;
};

RENDER_END