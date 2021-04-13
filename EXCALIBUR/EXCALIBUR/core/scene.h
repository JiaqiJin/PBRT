#pragma once

#include "Header.h"
#include "primitive.h"
#include "light.h"
#include "../tools/stats.h"

RENDERING_BEGIN

class Scene {
public:
	Scene(std::shared_ptr<Primitive> aggregate,
		const std::vector<std::shared_ptr<Light>>& lights)
		: lights(lights), aggregate(aggregate) {

	}

	bool Intersect(const Ray& ray, SurfaceInteraction* isect) const {
		return false; 
	} // todo

	bool IntersectP(const Ray& ray) const {
		return false;
	}

	std::vector<std::shared_ptr<Light>> lights;
private:
	std::shared_ptr<Primitive> aggregate;
	AABB3f worldBound;
};

RENDERING_END