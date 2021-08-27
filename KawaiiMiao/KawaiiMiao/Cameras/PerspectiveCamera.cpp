#include "PerspectiveCamera.h"

RENDER_BEGIN

RENDER_REGISTER_CLASS(PerspectiveCamera, "Perspective");

PerspectiveCamera::PerspectiveCamera(const APropertyTreeNode& node)
{
	const auto props = node.getPropertyList();
	Float _fov = props.getFloat("Fov");
	auto _eye = props.getVector3f("Eye");
	auto _focus = props.getVector3f("Focus");
	auto _up = props.getVector3f("WorldUp", Vector3f(0.f, 1.f, 0.f));
	m_cameraToWorld = inverse(lookAt(_eye, _focus, _up));
	m_cameraToScreen = perspective(_fov, 1e-2f, 1000.f);

	// Film 
	{
		const auto& filmNode = node.getPropertyChild("Film");
		m_film = Film::ptr(static_cast<Film*>(AObjectFactory::createInstance(filmNode.getTypeName(), filmNode)));
	}

	activate();
}

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