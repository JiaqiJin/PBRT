#include "diffuse.h"
#include "../core/Shape.h"

RENDERING_BEGIN

DiffuseAreaLight::DiffuseAreaLight(const Transform& LightToWorld,
	const MediumInterface& mediumInterface, const Spectrum& Le,
	int nSamples, const std::shared_ptr<Shape>& shape,
	bool twoSided = false)
	:AreaLight(LightToWorld, mediumInterface, nSamples),
	_Lemit(Le),
	_shape(shape),
	_twoSided(twoSided),
	_area(_shape->area()) {

}

RENDERING_END