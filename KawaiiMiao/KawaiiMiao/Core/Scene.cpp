#include "Scene.h"

RENDER_BEGIN

bool Scene::hit(const Ray& ray) const
{
	return false;
}

bool Scene::hit(const Ray& ray, SurfaceInteraction& isect) const
{
	return false;
}

RENDER_END