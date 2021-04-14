#pragma once

#include "../core/light.h"

RENDERING_BEGIN

class DiffuseAreaLight : public AreaLight {
public:
	DiffuseAreaLight(const Transform& LightToWorld,
		const MediumInterface& mediumInterface, const Spectrum& Le,
		int nSamples, const std::shared_ptr<Shape>& shape,
		bool twoSided = false);

private:
	// 面光源才有的，发射的辐射度
	const Spectrum _Lemit;
	std::shared_ptr<Shape> _shape;
	const Float _area;
	const bool _twoSided;
};

RENDERING_END