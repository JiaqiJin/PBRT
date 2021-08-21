#pragma once

#include "Rendering.h"
#include "Spectrum.h"
#include "../Math/KMathUtil.h"
#include "Rtti.h"

RENDER_BEGIN

class Material : public AObject
{
public:
	typedef std::shared_ptr<Material> ptr;

	Material() = default;
	virtual ~Material() = default;

	virtual void computeScatteringFunctions(SurfaceInteraction& si, MemoryArena& arena,
		TransportMode mode, bool allowMultipleLobes) const = 0;

	virtual ClassType getClassType() const override { return ClassType::RMaterial; }

	//static void Bump(const std::shared_ptr<Texture<Float>>& d, SurfaceInteraction* si);
};

RENDER_END