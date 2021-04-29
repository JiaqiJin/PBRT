#include "Camera.h"

RENDER_BEGIN

Camera::Camera(const Transform& cameraToWorld, Film::ptr film)
	:m_cameraToWorld(cameraToWorld),
	m_film(film)
{

}

Camera::~Camera() {}

PerspectiveCamera::PerspectiveCamera(
	const Transform& cameraToWorld,
	const Bounds2f& screenWindow,
	Float fov,
	Film::ptr film)
	: ProjectiveCamera(cameraToWorld, perspective(fov, 1e-2f, 1000.f), screenWindow, film)
{
	// Compute image plane bounds at $z=1$ for _PerspectiveCamera_
	Vector2i res = film->getResolution();
	Vector3f pMin = m_rasterToCamera(Vector3f(0, 0, 0), 1.0f);
	Vector3f pMax = m_rasterToCamera(Vector3f(res.x, res.y, 0), 1.0f);
	pMin /= pMin.z;
	pMax /= pMax.z;
	A = std::abs((pMax.x - pMin.x) * (pMax.y - pMin.y));
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