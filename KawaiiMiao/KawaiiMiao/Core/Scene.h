#pragma once

#include "Rendering.h"

RENDER_BEGIN

class Scene
{
public:
	typedef std::shared_ptr<Scene> ptr;

	Scene() {}

	bool hit(const Ray& ray) const;
	bool hit(const Ray& ray, SurfaceInteraction& isect) const;
};

RENDER_END