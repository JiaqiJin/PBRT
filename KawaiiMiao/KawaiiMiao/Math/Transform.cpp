#include "Transform.h"


#include <glm/glm/gtc/matrix_transform.hpp>
#include "../Core/Interaction.h"


RENDER_BEGIN

Transform translate(const Vector3f& delta)
{
	Matrix4x4 trans = glm::translate(Matrix4x4(1.0f), delta);
	Matrix4x4 transInv = glm::translate(Matrix4x4(1.0f), -delta);
	return Transform(trans, transInv);
}

Transform scale(Float x, Float y, Float z)
{
	Matrix4x4 trans = glm::scale(Matrix4x4(1.0f), Vector3f(x, y, z));
	Matrix4x4 transInv = glm::scale(Matrix4x4(1.0f), Vector3f(1 / x, 1 / y, 1 / z));
	return Transform(trans, transInv);
}

Transform rotateX(Float theta)
{
	Matrix4x4 trans = glm::rotate(Matrix4x4(1.0f), glm::radians(theta), Vector3f(1, 0, 0));
	Matrix4x4 transInv = inverse(trans);
	return Transform(trans, transInv);
}

Transform rotateY(Float theta)
{
	Matrix4x4 trans = glm::rotate(Matrix4x4(1.0f), glm::radians(theta), Vector3f(0, 1, 0));
	Matrix4x4 transInv = inverse(trans);
	return Transform(trans, transInv);
}

Transform rotateZ(Float theta)
{
	Matrix4x4 trans = glm::rotate(Matrix4x4(1.0f), glm::radians(theta), Vector3f(0, 0, 1));
	Matrix4x4 transInv = inverse(trans);
	return Transform(trans, transInv);
}

Transform rotate(Float theta, const Vector3f& axis)
{
	Matrix4x4 trans = glm::rotate(Matrix4x4(1.0f), glm::radians(theta), axis);
	Matrix4x4 transInv = inverse(trans);
	return Transform(trans, transInv);
}

Transform lookAt(const Vector3f& pos, const Vector3f& look, const Vector3f& up)
{
	Matrix4x4 worldToCamera = glm::lookAt(pos, look, up);
	return Transform(worldToCamera, inverse(worldToCamera));
}

Transform orthographic(Float znear, Float zfar)
{
	return scale(1, 1, 1 / (zfar - znear)) * translate(Vector3f(0, 0, -znear));
}

Transform perspective(Float fov, Float n, Float f)
{
	// Perform projective divide for perspective projection
	Matrix4x4 persp(
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, f / (f - n), 1,
		0, 0, -f * n / (f - n), 0);
	// Scale canonical perspective view to specified field of view
	Float invTanAng = 1 / glm::tan(glm::radians(fov) / 2);
	return scale(invTanAng, invTanAng, 1) * Transform(persp);
}

//-------------------------------------------Transform-------------------------------------

Transform Transform::operator*(const Transform& t2) const
{
	return Transform(mul(m_trans, t2.m_trans), mul(t2.m_transInv, m_transInv));
}

Bounds3f Transform::operator()(const Bounds3f& b) const
{
	const Transform& mat = *this;
	Bounds3f ret(mat(Vector3f(b.m_pMin.x, b.m_pMin.y, b.m_pMin.z), 1.0f));
	ret = unionBounds(ret, mat(Vector3f(b.m_pMax.x, b.m_pMin.y, b.m_pMin.z), 1.0f));
	ret = unionBounds(ret, mat(Vector3f(b.m_pMin.x, b.m_pMax.y, b.m_pMin.z), 1.0f));
	ret = unionBounds(ret, mat(Vector3f(b.m_pMin.x, b.m_pMin.y, b.m_pMax.z), 1.0f));
	ret = unionBounds(ret, mat(Vector3f(b.m_pMin.x, b.m_pMax.y, b.m_pMax.z), 1.0f));
	ret = unionBounds(ret, mat(Vector3f(b.m_pMax.x, b.m_pMax.y, b.m_pMin.z), 1.0f));
	ret = unionBounds(ret, mat(Vector3f(b.m_pMax.x, b.m_pMin.y, b.m_pMax.z), 1.0f));
	ret = unionBounds(ret, mat(Vector3f(b.m_pMax.x, b.m_pMax.y, b.m_pMax.z), 1.0f));
	return ret;
}

SurfaceInteraction Transform::operator()(const SurfaceInteraction& si) const
{
	SurfaceInteraction ret;
	//TODO
	return ret;
}

RENDER_END