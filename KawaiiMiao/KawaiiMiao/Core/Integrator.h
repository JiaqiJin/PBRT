#pragma once

#include "Rendering.h"
#include "Sampler.h"
#include "Sampling.h"
#include "Camera.h"
#include "Primitive.h"

RENDER_BEGIN

class Integrator
{
public:
	typedef std::shared_ptr<Integrator> ptr;

	virtual ~Integrator() = default;
	virtual void render(const Scene& scene) = 0;
};

class SamplerIntegrator : public Integrator
{
public:
	typedef std::shared_ptr<SamplerIntegrator> ptr;

	SamplerIntegrator(Camera::ptr camera, Sampler::ptr sampler, const Bounds2i& pixelBounds)
		: m_camera(camera), m_sampler(sampler), m_pixelBounds(pixelBounds) {}

	virtual void preprocess(const Scene& scene, Sampler::ptr sampler) {}

	virtual void render(const Scene& scene);

	virtual Spectrum Li(const Ray& ray, const Scene& scene,
		Sampler& sampler, MemoryArena& arena, int depth = 0) const = 0;

	Spectrum specularReflect(const Ray& ray, const SurfaceInteraction& isect,
		const Scene& scene, Sampler& sampler, MemoryArena& arena, int depth) const;

	Spectrum specularTransmit(const Ray& ray, const SurfaceInteraction& isect,
		const Scene& scene, Sampler& sampler, MemoryArena& arena, int depth) const;

protected:
	Camera::ptr m_camera;

private:
	Sampler::ptr m_sampler;
	const Bounds2i m_pixelBounds;
};



RENDER_END