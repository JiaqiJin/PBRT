#pragma once

#include "../Core/Rendering.h"

#include <glm/glm/common.hpp>
#include <glm/glm/gtc/quaternion.hpp>

RENDERING_BEGIN

template <typename T>
inline bool isNaN(const T x) { return std::isnan(x); }

template <>
inline bool isNaN(const int x) { return false; }

inline Float radians(Float angle)
{
	return angle * Pi / 180.0f;
}

inline Float angles(Float radians)
{
	return radians * 180.0f / Pi;
}

inline bool equal(Float a, Float b)
{
	return fabs(a - b) < MachineEpsilon;
}

// Vector
template<typename T>
using Vector2 = glm::vec<2, T>;
template<typename T>
using Vector3 = glm::vec<3, T>;

typedef Vector2<Float> AVector2f;
typedef Vector2<int> AVector2i;
typedef Vector3<Float> AVector3f;
typedef Vector3<int> AVector3i;

template <typename T>
inline std::ostream& operator<<(std::ostream& os, const Vector2<T>& v)
{
	os << "[ " << v.x << ", " << v.y << " ]";
	return os;
}

template <typename T>
inline std::ostream& operator<<(std::ostream& os, const Vector3<T>& v)
{
	os << "[ " << v.x << ", " << v.y << ", " << v.z << " ]";
	return os;
}

// Matrix
using Matrix4x4 = glm::mat<4, 4, Float>;
using Quaterion = glm::qua<Float>;

inline Matrix4x4 transpose(const Matrix4x4& m) { return glm::transpose(m); }
inline Matrix4x4 mul(const Matrix4x4& m1, const Matrix4x4& m2) { return m1 * m2; }
inline Matrix4x4 inverse(const Matrix4x4& m) { return glm::inverse(m); }

inline Matrix4x4 toMatrix4x4(const Quaterion& q) { return glm::mat4_cast(q); }

RENDERING_END