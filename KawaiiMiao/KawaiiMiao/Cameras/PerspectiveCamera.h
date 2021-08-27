#pragma once

#include "../Core/Camera.h"

RENDER_BEGIN

class PerspectiveCamera final : public ProjectiveCamera
{
public:
	typedef std::shared_ptr<PerspectiveCamera> ptr;

	PerspectiveCamera(const APropertyTreeNode& node);
	PerspectiveCamera(const Transform& CameraToWorld, Float fov, Film::ptr film);

	virtual Float castingRay(const CameraSample& sample, Ray& ray) const override;

	virtual void activate() override { initialize(); }

	virtual std::string toString() const override { return "PerspectiveCamera[]"; }

protected:
	virtual void initialize() override;

private:
	//Vector3f dxCamera, dyCamera;
	Float A;
};

RENDER_END