#pragma once

#include "../Interaction.h"
#include "BxDF.h"
#include "../spectrum.h"

RENDERING_BEGIN

/*
* 入射点跟出射点距离很近，可以简化模型，当做一个点处理，我们可以用BRDF来描述
*  但如果由于材质原因（半透明，例如，人的皮肤，蜡烛），入射点与与出射点的距离较远 能当做一个点来处理，则需要BSSRDF来表示了
* Lo(po,ωo) = ∫[A] ∫[H] S(po, ωo, pi, ωi) Li(pi, ωi) |cosθi| dωidA    1式
*/
class BSSRDF {
public:
	BSSRDF(const SurfaceInteraction& po, Float eta)
		: _po(po),
		_eta(eta) { }

	virtual ~BSSRDF() { }

	virtual Spectrum S(const SurfaceInteraction& pi, const Vector3f& wi) = 0;
	virtual Spectrum sampleS(const Scene& scene, Float u1, const Point2f& u2,
		MemoryArena& arena, SurfaceInteraction* si,
		Float* pdf) const = 0;

protected:
	// 光线出射的位置，包含了出射方向 ωo
	const SurfaceInteraction& _po;
	// 折射率
	Float _eta;
};

RENDERING_END