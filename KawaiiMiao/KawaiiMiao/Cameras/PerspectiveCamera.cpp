#include "PerspectiveCamera.h"

RENDER_BEGIN

PerspectiveCamera::PerspectiveCamera(const Transform& CameraToWorld, Float fov, Film::ptr film)
	: ProjectiveCamera(CameraToWorld, perspective(fov, 1e-2f, 1000.f), film)
{
	initialize();
}

void PerspectiveCamera::initialize()
{
	// Compute image plane bounds at $z=1$ for _PerspectiveCamera_
	Vector2i res = m_film->getResolution();
	Vector3f pMin = m_rasterToCamera(Vector3f(0, 0, 0), 1.0f);
	Vector3f pMax = m_rasterToCamera(Vector3f(res.x, res.y, 0), 1.0f);
	pMin /= pMin.z;
	pMax /= pMax.z;
	A = glm::abs((pMax.x - pMin.x) * (pMax.y - pMin.y));

	ProjectiveCamera::initialize();
}

Float PerspectiveCamera::castingRay(const CameraSample& sample, Ray& ray) const
{
	// Compute raster and camera sample positions
	Vector3f pFilm = Vector3f(sample.pFilm.x, sample.pFilm.y, 0);
	Vector3f pCamera = m_rasterToCamera(pFilm, 1.0f);
	ray = Ray(Vector3f(0, 0, 0), normalize(Vector3f(pCamera)));
	ray = m_cameraToWorld(ray);
	return 1.f;
}

RENDER_END