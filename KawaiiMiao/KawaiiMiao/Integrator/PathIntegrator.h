#pragma once

#include "../Core/Integrator.h"
#include "../Core/LightDistrib.h"

RENDER_BEGIN

class PathIntegrator : public SamplerIntegrator
{
public:

	PathIntegrator(const APropertyTreeNode& props);

	PathIntegrator(int maxDepth, Camera::ptr camera, Sampler::ptr sampler,
		Float rrThreshold = 1, const std::string& lightSampleStrategy = "spatial");

	virtual void preprocess(const Scene& scene) override;

	virtual Spectrum Li(const Ray& ray, const Scene& scene, Sampler& sampler,
		MemoryArena& arena, int depth) const override;

	virtual std::string toString() const override { return "PathIntegrator[]"; }

private:
	// PathIntegrator Private Data
	int m_maxDepth;
	Float m_rrThreshold;
	std::string m_lightSampleStrategy;
	std::unique_ptr<LightDistribution> m_lightDistribution;
};

RENDER_END