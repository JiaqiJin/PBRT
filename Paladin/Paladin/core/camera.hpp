#ifndef camera_hpp
#define camera_hpp

#include "header.h"
#include "film.hpp"
#include "../math/animatedtransform.hpp"

PALADIN_BEGIN

class Camera
{
public:
	Camera(const AnimatedTransform& CameraToWorld, Float shutterOpen,
		Float shutterClose, Film* film, const Medium* medium);
	virtual ~Camera();
	virtual Float GenerateRay(const CameraSample &sample, Ray *ray) const = 0;
	virtual Float GenerateRayDifferential(const CameraSample& sample,
		RayDifferential* rd) const;
	virtual Spectrum We(const Ray& ray, Point2f* pRaster2 = nullptr) const;
	virtual void Pdf_We(const Ray& ray, Float* pdfPos, Float* pdfDir) const;
	virtual Spectrum Sample_Wi(const Interaction& ref, const Point2f& u,
		Vector3f* wi, Float* pdf, Point2f* pRaster,
		VisibilityTester* vis) const;


	AnimatedTransform cameraToWorld;
	const Float shutterOpen, shutterClose;
	Film* film;
	//Scattering medium.
	const Medium* medium;
};

struct CameraSample
{
	// point on the film which generate ray carries radiance
	Point2f pFilm; 
	// point of lenght the ray passes 
	Point2f pLens;
	Float time;
};

PALADIN_END

#endif /* camera_hpp */
