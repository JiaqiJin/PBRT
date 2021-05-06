#include "Scene.h"

RENDER_BEGIN

bool Scene::hit(const Ray& ray) const
{
	return m_aggreShape->hit(ray);
}

bool Scene::hit(const Ray& ray, SurfaceInteraction& isect) const
{
	return m_aggreShape->hit(ray, isect);
}

bool Scene::hitTr(Ray ray, Sampler& sampler, SurfaceInteraction& isect, Spectrum& Tr) const
{
	Tr = Spectrum(1.f);
	//while (true) {
	//	bool hitSurface = hit(ray, isect);
	//	// Accumulate beam transmittance for ray segment
	//	if (ray.medium) *Tr *= ray.medium->Tr(ray, sampler);

	//	// Initialize next ray segment or terminate transmittance computation
	//	if (!hitSurface) return false;
	//	if (isect->primitive->GetMaterial() != nullptr) return true;
	//	ray = isect->SpawnRay(ray.d);
	//}
	return false;
}

RENDER_END