#pragma once

#include "Rendering.h"
#include "Spectrum.h"
#include "../Math/KMathUtil.h"

RENDER_BEGIN

class Material
{
public:
	typedef std::shared_ptr<Material> ptr;

	virtual void computeScatteringFunctions(SurfaceInteraction& si, MemoryArena& arena,
		TransportMode mode, bool allowMultipleLobes) const = 0;
};

RENDER_END