#pragma once

#include "../core/Header.h"
#include "../core/Material.h"
#include "../core/BXDF/BSDF.h"
#include "../core/BXDF/Lambertian.h"
#include "../parallel/Parallel.h"

RENDERING_BEGIN

class MixMaterial : public Material {
public:
	MixMaterial(const std::shared_ptr<Material>& m1,
		const std::shared_ptr<Material>& m2,
		const std::shared_ptr<Texture<Spectrum>>& scale)
		: m1(m1), m2(m2), scale(scale) { }

	virtual void ComputeScatteringFunctions(SurfaceInteraction* si,
		MemoryArena& arena,
		TransportMode mode,
		bool allowMultipleLobes) const override;

private:
	std::shared_ptr<Material> m1, m2;
	std::shared_ptr<Texture<Spectrum>> scale;
};

RENDERING_END