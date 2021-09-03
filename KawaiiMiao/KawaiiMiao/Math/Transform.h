#pragma once

#include "../Core/Rendering.h"
#include "KMathUtil.h"

RENDER_BEGIN

class Transform
{
public:
	typedef std::shared_ptr<Transform> ptr;

	Transform() : m_trans(Matrix4x4(1.0f)), m_transInv(Matrix4x4(1.0f)) {}

	Transform(const Float mat[4][4])
	{
		//Note: column major matrix
		m_trans = Matrix4x4(
			mat[0][0], mat[1][0], mat[2][0], mat[3][0],
			mat[0][1], mat[1][1], mat[2][1], mat[3][1],
			mat[0][2], mat[1][2], mat[2][2], mat[3][2],
			mat[0][3], mat[1][3], mat[2][3], mat[3][3]);
		m_transInv = inverse(m_trans);
	}

	Transform(const Matrix4x4& m) : m_trans(m), m_transInv(inverse(m)) {}
	Transform(const Matrix4x4& m, const Matrix4x4& mInv) : m_trans(m), m_transInv(mInv) {}

	friend Transform inverse(const Transform& t) { return Transform(t.m_transInv, t.m_trans); }
	friend Transform transpose(const Transform& t) { return Transform(transpose(t.m_trans), transpose(t.m_trans)); }

	bool operator==(const Transform& t) const { return t.m_trans == m_trans && t.m_transInv == m_transInv; }
	bool operator!=(const Transform& t) const { return t.m_trans != m_trans || t.m_transInv != m_transInv; }

	//Ray
	inline Ray operator()(const Ray& r) const;
	//Bounds
	Bounds3f operator()(const Bounds3f& b) const;
	//SurfaceInteraction
	SurfaceInteraction operator()(const SurfaceInteraction& si) const;
	//Vector
	template <typename T>
	inline Vector3<T> operator()(const Vector3<T>& p, const Float& w) const;

	bool isIdentity() const
	{
		return (
			m_trans[0][0] == 1.f && m_trans[1][0] == 0.f && m_trans[2][0] == 0.f && m_trans[3][0] == 0.f &&
			m_trans[0][1] == 0.f && m_trans[1][1] == 1.f && m_trans[2][1] == 0.f && m_trans[3][1] == 0.f &&
			m_trans[0][2] == 0.f && m_trans[1][2] == 0.f && m_trans[2][2] == 1.f && m_trans[3][2] == 0.f &&
			m_trans[0][3] == 0.f && m_trans[1][3] == 0.f && m_trans[2][3] == 0.f && m_trans[3][3] == 1.f);
	}

	const Matrix4x4& getMatrix() const { return m_trans; }
	const Matrix4x4& getInverseMatrix() const { return m_transInv; }

	Transform operator*(const Transform& t2) const;

	bool HasScale() const
	{
		Float la2 = lengthSquared((Vector3f(1, 0, 0)));
		Float lb2 = lengthSquared((Vector3f(0, 1, 0)));
		Float lc2 = lengthSquared((Vector3f(0, 0, 1)));
#define NOT_ONE(x) ((x) < .999f || (x) > 1.001f)
		return (NOT_ONE(la2) || NOT_ONE(lb2) || NOT_ONE(lc2));
#undef NOT_ONE
	}
private:
	Matrix4x4 m_trans, m_transInv;
	friend class AnimatedTransform;
};

Transform translate(const Vector3f& delta);
Transform scale(Float x, Float y, Float z);
Transform rotateX(Float theta);
Transform rotateY(Float theta);
Transform rotateZ(Float theta);
Transform rotate(Float theta, const Vector3f& axis);
Transform lookAt(const Vector3f& pos, const Vector3f& look, const Vector3f& up);
Transform orthographic(Float znear, Float zfar);
Transform perspective(Float fov, Float znear, Float zfar);

template <typename T>
inline Vector3<T> Transform::operator()(const Vector3<T>& p, const Float& w) const
{
	//Note: w == 1.f -> point, w == 0.f -> vector
	glm::vec<4, Float> ret = m_trans * glm::vec<4, Float>(p.x, p.y, p.z, w);
	if (w == 0.f)
		return Vector3<T>(ret.x, ret.y, ret.z);

	CHECK_NE(ret.w, 0);
	if (ret.w == 1)
		return Vector3<T>(ret.x, ret.y, ret.z);
	else
		return Vector3<T>(ret.x, ret.y, ret.z) / ret.w;
}

inline Ray Transform::operator()(const Ray& r) const
{
	Vector3f o = (*this)(r.m_origin, 1.0f);
	Vector3f d = (*this)(r.m_dir, 0.0f);
	Float tMax = r.m_tMax;

	return Ray(o, d, tMax);
}

//// AnimatedTransform 
//class AnimatedTransform
//{
//public:
//	AnimatedTransform(const Transform* startTransform, Float startTime,
//		const Transform* endTransform, Float endTime);
//	static void Decompose(const Matrix4x4& m, Vector3f* T, Quaternion* R, Matrix4x4* S);
//	void Interpolate(Float time, Transform* t) const;
//	Ray operator()(const Ray& r) const;
//	RayDifferential operator()(const RayDifferential& r) const;
//	Vector3f operator()(Float time, const Vector3f& v) const;
//	bool HasScale() const 
//	{
//		return startTransform->HasScale() || endTransform->HasScale();
//	}
//	Bounds3f MotionBounds(const Bounds3f& b) const;
//	Bounds3f BoundPointMotion(const Vector3f& p) const;
//private:
//	// AnimatedTransform Private Data
//	const Transform* startTransform, * endTransform;
//	const Float startTime, endTime;
//	const bool actuallyAnimated;
//	Vector3f T[2];
//	Quaternion R[2];
//	Matrix4x4 S[2];
//	bool hasRotation;
//	struct DerivativeTerm {
//		DerivativeTerm() {}
//		DerivativeTerm(Float c, Float x, Float y, Float z)
//			: kc(c), kx(x), ky(y), kz(z) {}
//		Float kc, kx, ky, kz;
//		Float Eval(const Vector3f& p) const
//		{
//			return kc + kx * p.x + ky * p.y + kz * p.z;
//		}
//	};
//	DerivativeTerm c1[3], c2[3], c3[3], c4[3], c5[3];
//};

RENDER_END