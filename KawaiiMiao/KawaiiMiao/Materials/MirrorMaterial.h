#pragma once

#include "../Core/Material.h"

RENDER_BEGIN

class MirrorMaterial final : public Material
{
public:
	typedef std::shared_ptr<MirrorMaterial> ptr;

	MirrorMaterial(const Spectrum& r) : m_Kr(r) {}

	virtual void computeScatteringFunctions(SurfaceInteraction& si, MemoryArena& arena,
		TransportMode mode, bool allowMultipleLobes) const override;

private:
	Spectrum m_Kr;
};

RENDER_END