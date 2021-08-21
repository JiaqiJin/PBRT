#pragma once

#include "Rendering.h"
#include "Sampler.h"
#include "Sampling.h"
#include "Camera.h"
#include "Primitive.h"
#include "Rtti.h"

RENDER_BEGIN

class Integrator : public AObject
{
public:
	typedef std::shared_ptr<Integrator> ptr;

	virtual ~Integrator() = default;

	virtual void preprocess(const Scene & scene) = 0;
	virtual void render(const Scene & scene) = 0;

	virtual ClassType getClassType() const override { return ClassType::RIntegrator; }
};

class SamplerIntegrator : public Integrator
{
public:
	typedef std::shared_ptr<SamplerIntegrator> ptr;

	// SamplerIntegrator Public Methods
	SamplerIntegrator(Camera::ptr camera, Sampler::ptr sampler)
		: m_camera(camera), m_sampler(sampler) {}

	virtual void preprocess(const Scene& scene) override {}

	virtual void render(const Scene& scene) override;

	virtual Spectrum Li(const Ray& ray, const Scene& scene,
		Sampler& sampler, MemoryArena& arena, int depth = 0) const = 0;

	Spectrum specularReflect(const Ray& ray, const SurfaceInteraction& isect,
		const Scene& scene, Sampler& sampler, MemoryArena& arena, int depth) const;

	Spectrum specularTransmit(const Ray& ray, const SurfaceInteraction& isect,
		const Scene& scene, Sampler& sampler, MemoryArena& arena, int depth) const;

protected:
	Camera::ptr m_camera;
	Sampler::ptr m_sampler;
};


Spectrum uiformSampleAllLights(const Interaction& it, const Scene& scene,
	MemoryArena& arena, Sampler& sampler, const std::vector<int>& nLightSamples);

Spectrum uniformSampleOneLight(const Interaction& it, const Scene& scene,
	MemoryArena& arena, Sampler& sampler, const Distribution1D* lightDistrib);

Spectrum estimateDirect(const Interaction& it, const Vector2f& uShading, const Light& light,
	const Vector2f& uLight, const Scene& scene, Sampler& sampler, MemoryArena& arena, bool specular = false);

RENDER_END