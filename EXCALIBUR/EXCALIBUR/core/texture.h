#pragma once

#include "Header.h"
#include "../math/transform.h"
#include "Interaction.h"
#include "spectrum.h"

RENDERING_BEGIN

class TextureMapping2D {
public:
	virtual ~TextureMapping2D() {}

	virtual Point2f map(const SurfaceInteraction& si, Vector2f* dstdx, Vector2f* dstdy) const = 0;
};

// uv映射 将传入的u,v坐标乘以一个系数再加上一个偏移量就得到了s,t坐标
class UVMapping2D : public TextureMapping2D {
public:
	UVMapping2D(Float su = 1, Float sv = 1, Float du = 0, Float dv = 0)
		: su(su), sv(sv), du(du), dv(dv) { }

	virtual Point2f map(const SurfaceInteraction& si, Vector2f* dstdx, Vector2f* dstdy)	const;

private:
	const Float su, sv, du, dv;
};

class SphericalMapping2D : public TextureMapping2D {
public:
	SphericalMapping2D(const Transform& worldToTexture)
		: _worldToTexture(worldToTexture) { }

	virtual Point2f map(const SurfaceInteraction& si, Vector2f* dstdx, Vector2f* dstdy) const;
private:
	Point2f pointToSphereToST(const Point3f& p) const;

	const Transform _worldToTexture;
};

// add more mapping methods

class TextureMapping3D {
public:
	virtual ~TextureMapping3D() {}

	virtual Point3f map(const SurfaceInteraction& si, Vector3f* dpdx, Vector3f* dpdy) const = 0;
};

class TransformMapping3D : public TextureMapping3D {
public:
	TransformMapping3D(const Transform& worldToTexture)
		: _worldToTexture(worldToTexture) {}

	virtual Point3f map(const SurfaceInteraction& si, Vector3f* dpdx, Vector3f* dpdy) const;
private:
	const Transform _worldToTexture;
};

template<typename T>
class Texture {
public:
	virtual T evaluate(const SurfaceInteraction&) const = 0;

	virtual ~Texture() { }
};

RENDERING_END