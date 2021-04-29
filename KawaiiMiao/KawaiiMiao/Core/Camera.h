#pragma once

#include "Rendering.h"
#include "Film.h"
#include "../Math/Transform.h"

RENDER_BEGIN

struct CameraSample
{
	Vector2f pFilm;
};

inline std::ostream& operator<<(std::ostream& os, const CameraSample& cs)
{
	os << "[ pFilm: " << cs.pFilm << " ]";
	return os;
}

class Camera
{
public:
	typedef std::shared_ptr<Camera> ptr;

	// Camera Interface
	Camera(const Transform& cameraToWorld, Film::ptr film);
	virtual ~Camera();

	virtual Float castingRay(const CameraSample& sample, Ray& ray) const = 0;

	Transform m_cameraToWorld;
	Film::ptr m_film;
};

class ProjectiveCamera : public Camera
{
public:
	typedef std::shared_ptr<ProjectiveCamera> ptr;

	ProjectiveCamera(const Transform& cameraToWorld,
		const Transform& cameraToScreen, const Bounds2f& screenWindow, Film::ptr film)
		: Camera(cameraToWorld, film), m_cameraToScreen(cameraToScreen)
	{
		// Compute projective camera screen transformations
		auto resolution = film->getResolution();
		m_screenToRaster = scale(resolution.x, resolution.y, 1) *
			scale(1 / (screenWindow.m_pMax.x - screenWindow.m_pMin.x),
				1 / (screenWindow.m_pMin.y - screenWindow.m_pMax.y), 1) *
			translate(Vector3f(-screenWindow.m_pMin.x, -screenWindow.m_pMax.y, 0));
		m_rasterToScreen = inverse(m_screenToRaster);
		m_rasterToCamera = inverse(m_cameraToScreen) * m_rasterToScreen;
	}

protected:
	Transform m_cameraToScreen, m_rasterToCamera;
	Transform m_screenToRaster, m_rasterToScreen;
};

class PerspectiveCamera final : public ProjectiveCamera
{
public:
	typedef std::shared_ptr<PerspectiveCamera> ptr;

	PerspectiveCamera(const Transform& CameraToWorld, const Bounds2f& screenWindow,
		Float fov, Film::ptr film);

	virtual Float castingRay(const CameraSample& sample, Ray& ray) const override;

	//Spectrum We(const Ray &ray, Point2f *pRaster2 = nullptr) const;
	//void Pdf_We(const Ray &ray, Float *pdfPos, Float *pdfDir) const;
	//Spectrum Sample_Wi(const Interaction &ref, const Point2f &sample,
	//	Vector3f *wi, Float *pdf, Point2f *pRaster,
	//	VisibilityTester *vis) const;

private:
	Float A;
};


RENDER_END