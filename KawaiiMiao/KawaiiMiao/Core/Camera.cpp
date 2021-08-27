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

RENDER_END