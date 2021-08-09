#include "Camera.h"

RENDER_BEGIN

Camera::Camera(const Transform& cameraToWorld, Film::ptr film)
	:m_cameraToWorld(cameraToWorld),
	m_film(film)
{

}

Camera::~Camera() {}

// ---------------------- Projective Camera ----------------------

void ProjectiveCamera::initialize()
{
	// Compute projective camera screen transformations
	Bounds2f screen;
	auto res = m_film->getResolution();
	Float frame = (Float)(res.x) / res.y;
	if (frame > 1.f)
	{
		screen.m_pMin.x = -frame;
		screen.m_pMax.x = frame;
		screen.m_pMin.y = -1.f;
		screen.m_pMax.y = 1.f;
	}
	else
	{
		screen.m_pMin.x = -1.f;
		screen.m_pMax.x = 1.f;
		screen.m_pMin.y = -1.f / frame;
		screen.m_pMax.y = 1.f / frame;
	}

	m_screenToRaster = scale(res.x, res.y, 1) *
		scale(1 / (screen.m_pMax.x - screen.m_pMin.x),
			1 / (screen.m_pMin.y - screen.m_pMax.y), 1) *
		translate(Vector3f(-screen.m_pMin.x, -screen.m_pMax.y, 0));
	m_rasterToScreen = inverse(m_screenToRaster);
	m_rasterToCamera = inverse(m_cameraToScreen) * m_rasterToScreen;
}

// ------------------- Perspective Camera ----------------
//
//PerspectiveCamera::PerspectiveCamera(
//	const Transform& cameraToWorld,
//	const Bounds2f& screenWindow,
//	Float fov,
//	Film::ptr film)
//	: ProjectiveCamera(cameraToWorld, perspective(fov, 1e-2f, 1000.f), screenWindow, film)
//{
//	// Compute image plane bounds at $z=1$ for _PerspectiveCamera_
//	Vector2i res = film->getResolution();
//	Vector3f pMin = m_rasterToCamera(Vector3f(0, 0, 0), 1.0f);
//	Vector3f pMax = m_rasterToCamera(Vector3f(res.x, res.y, 0), 1.0f);
//	pMin /= pMin.z;
//	pMax /= pMax.z;
//	A = std::abs((pMax.x - pMin.x) * (pMax.y - pMin.y));
//}
//
//Float PerspectiveCamera::castingRay(const CameraSample& sample, Ray& ray) const
//{
//	// Compute raster and camera sample positions
//	Vector3f pFilm = Vector3f(sample.pFilm.x, sample.pFilm.y, 0);
//	Vector3f pCamera = m_rasterToCamera(pFilm, 1.0f);
//	ray = Ray(Vector3f(0, 0, 0), normalize(Vector3f(pCamera)));
//	ray = m_cameraToWorld(ray);
//	return 1.f;
//}

RENDER_END