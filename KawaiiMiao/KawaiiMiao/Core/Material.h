#pragma once

#include "Rendering.h"
#include "Spectrum.h"
#include "../Math/KMathUtil.h"

RENDER_BEGIN

class Material
{
public:
	typedef std::shared_ptr<Material> ptr;

	Material() = default;
	virtual ~Material() = default;

	virtual void computeScatteringFunctions(SurfaceInteraction& si, MemoryArena& arena,
		TransportMode mode, bool allowMultipleLobes) const = 0;

	//static void Bump(const std::shared_ptr<Texture<Float>>& d, SurfaceInteraction* si);
};

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

class LambertianMaterial final : public Material
{
public:
	typedef std::shared_ptr<LambertianMaterial> ptr;

	LambertianMaterial(const Spectrum& r) : m_Kr(r) {}

	virtual void computeScatteringFunctions(SurfaceInteraction& si, MemoryArena& arena,
		TransportMode mode, bool allowMultipleLobes) const override;

private:
	Spectrum m_Kr;
};


RENDER_END