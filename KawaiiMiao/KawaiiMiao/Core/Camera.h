#pragma once

#include "Rendering.h"
#include "Film.h"
#include "../Math/Transform.h"
#include "Rtti.h"

RENDER_BEGIN

// Holds all of the sample values needed to specify a camera ray
struct CameraSample
{
	Vector2f pFilm;
	//Vector2f pLens;
	//Float time;
};

inline std::ostream& operator<<(std::ostream& os, const CameraSample& cs)
{
	os << "[ pFilm: " << cs.pFilm << " ]";
	return os;
}

class Camera : public AObject
{
public:
	typedef std::shared_ptr<Camera> ptr;

	// Camera Interface
	Camera() = default;
	Camera(const Transform& cameraToWorld, Film::ptr film);
	virtual ~Camera();

	//Spectrum We(const Ray &ray, Point2f *pRaster2 = nullptr) const;
	//void Pdf_We(const Ray &ray, Float *pdfPos, Float *pdfDir) const;
	//Spectrum Sample_Wi(const Interaction &ref, const Point2f &sample,
	//	Vector3f *wi, Float *pdf, Point2f *pRaster,
	//	VisibilityTester *vis) const;

	// Compute the ray corresponding to a giving sample
	virtual Float castingRay(const CameraSample& sample, Ray& ray) const = 0;

	virtual ClassType getClassType() const override { return ClassType::RCamera; }

	// Camera Public Data
	Transform m_cameraToWorld;
	Film::ptr m_film;
};

class ProjectiveCamera : public Camera
{
public:
	typedef std::shared_ptr<ProjectiveCamera> ptr;

	ProjectiveCamera() = default;
	ProjectiveCamera(const Transform & cameraToWorld, const Transform & cameraToScreen, Film::ptr film)
		: Camera(cameraToWorld, film), m_cameraToScreen(cameraToScreen) { }

protected:
	virtual void initialize();

protected:
	Transform m_cameraToScreen, m_rasterToCamera;
	Transform m_screenToRaster, m_rasterToScreen;
};

//class PerspectiveCamera final : public ProjectiveCamera
//{
//public:
//	typedef std::shared_ptr<PerspectiveCamera> ptr;
//
//	PerspectiveCamera(const Transform& CameraToWorld, const Bounds2f& screenWindow,
//		Float fov, Film::ptr film);
//
//	virtual Float castingRay(const CameraSample& sample, Ray& ray) const override;
//
//private:
//	Float A;
//};


RENDER_END