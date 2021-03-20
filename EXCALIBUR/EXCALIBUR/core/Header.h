#pragma once

#include <iostream>
#include <assert.h>
#include <cmath>
#include <stdlib.h>
#include <vector>
#include <map>
#include <algorithm>
#include <iomanip>
#include <map>

#include <string>
#include <algorithm>
#include <memory>
#include <atomic>
#include <mutex>
#include <cctype>
#include "../tools/Logging.h"

#include "../ext/json.hpp"
typedef nlohmann::json nloJson;

#include "../tools/stringprint.h"
#include "../tools/macro.h"

namespace Rendering
{
	template<typename T>
	class Point2;

	template<typename T>
	class Point3;

	template<typename T>
	class Vector3;

	template<typename T>
	class Vector2;

	template<typename T>
	class Normal3;

	template<typename T>
	class AABB3;

	template<typename T>
	class AABB2;

	template<typename T>
	class Direction3;

	class Medium;

	class RGBSpectrum;

	class SampledSpectrum;

	class CObject;

#ifdef RENDERING_SAMPLED_SPECTRUM
	typedef SampledSpectrum Spectrum;
#else
	typedef RGBSpectrum Spectrum;
#endif

	class Ray;

	class Transform;

	class AnimatedTransform;

	class Shape;

	class MediumInteraction;

	class MemoryArena;

	class Primitive;

	class BSDF;

	class BxDF;

	class VisibilityTester;

	class Sampler;

	class Film;

	class Filter;

	class Aggregate;

	class Scene;

	class Integrator;

	class BSSRDF;

	struct Quaternion;

	struct Interaction;

	class SurfaceInteraction;

	struct MediumInterface;

	template<typename T, int logBlockSize = 2>
	class BlockedArray;

	class EFloat;

	class Light;

	class AreaLight;

	class Shape;

	class Material;

	class Camera;

	struct CameraSample;

	template <typename T>
	class Texture;

	class MixMaterial;

	struct Distribution1D;

	struct Distribution2D;

	struct DirectSamplingRecord;

	struct PositionSamplingRecord;

	class EnvironmentMap;


}

inline uint32_t floatToBits(float f) {
	uint32_t ui;
	memcpy(&ui, &f, sizeof(float));
	return ui;
}

inline int32_t floatToInt(float f) {
	int32_t i;
	memcpy(&i, &f, sizeof(float));
	return i;
}

inline float bitsToFloat(uint32_t ui) {
	float f;
	memcpy(&f, &ui, sizeof(uint32_t));
	return f;
}

inline float intToFloat(int32_t i) {
	float f;
	memcpy(&f, &i, sizeof(int32_t));
	return f;
}

inline uint64_t floatToBits(double f) {
	uint64_t ui;
	memcpy(&ui, &f, sizeof(double));
	return ui;
}

inline int64_t floatToInt(double f) {
	int64_t i;
	memcpy(&i, &f, sizeof(double));
	return i;
}


inline double bitsToFloat(uint64_t ui) {
	double f;
	memcpy(&f, &ui, sizeof(uint64_t));
	return f;
}

inline double intToFloat(int64_t i) {
	double f;
	memcpy(&f, &i, sizeof(int64_t));
	return f;
}

inline float nextFloatUp(float v) {
	if (std::isinf(v) && v > 0.) return v;
	if (v == -0.f) v = 0.f;
	uint32_t ui = floatToBits(v);
	if (v >= 0)
		++ui;
	else
		--ui;
	return bitsToFloat(ui);
}

inline float nextFloatDown(float v) {
	if (std::isinf(v) && v < 0.) return v;
	if (v == 0.f) v = -0.f;
	uint32_t ui = floatToBits(v);
	if (v > 0)
		--ui;
	else
		++ui;
	return bitsToFloat(ui);
}

inline double nextFloatUp(double v, int delta = 1) {
	if (std::isinf(v) && v > 0.) return v;
	if (v == -0.f) v = 0.f;
	uint64_t ui = floatToBits(v);
	if (v >= 0.)
		ui += delta;
	else
		ui -= delta;
	return bitsToFloat(ui);
}

inline double nextFloatDown(double v, int delta = 1) {
	if (std::isinf(v) && v < 0.) return v;
	if (v == 0.f) v = -0.f;
	uint64_t ui = floatToBits(v);
	if (v > 0.)
		ui -= delta;
	else
		ui += delta;
	return bitsToFloat(ui);
}

template <typename T>
inline T Mod(T a, T b) {
	T result = a - (a / b) * b;
	return (T)((result < 0) ? result + b : result);
}

template <>
inline Float Mod(Float a, Float b) {
	return std::fmod(a, b);
}

inline Float gammaCorrect(Float value) {
	if (value <= 0.0031308f) {
		return 12.92f * value;
	}
	return 1.055f * std::pow(value, (Float)(1.f / 2.4f)) - 0.055f;
}

inline Float inverseGammaCorrect(Float value) {
	if (value <= 0.04045f) {
		return value * 1.f / 12.92f;
	}
	return std::pow((value + 0.055f) * 1.f / 1.055f, (Float)2.4f);
}

// std的log是以e为底数 log2(x) = lnx / ln2
inline Float Log2(Float x) {
	const Float invLog2 = 1.442695040888963387004650940071;
	return std::log(x) * invLog2;
}

template <typename T>
inline CONSTEXPR bool isPowerOf2(T v) {
	return v && !(v & (v - 1));
}

#define ALLOCA(TYPE, COUNT) (TYPE *) alloca((COUNT) * sizeof(TYPE))


#include "../math/mathutil.h"

template <typename Predicate>
int findInterval(int size, const Predicate& pred) {
	int first = 0, len = size;
	while (len > 0) {
		int half = len >> 1, middle = first + half;
		// 二分法查找
		if (pred(middle)) {
			first = middle + 1;
			len -= half + 1;
		}
		else {
			len = half;
		}
	}
	return Rendering::clamp(first - 1, 0, size - 2);
}

#include "../math/vector.h"
#include "../math/point.h"
#include "../math/ray.h"
#include "../math/bounds.h"
#include "../tools/memory.h"
#include "../tools/errfloat.h"
#include "spectrum.h"
#include "cobject.h"

namespace Rendering 
{
	inline Float sphericalTheta(const Vector3f& v) {
		return std::acos(clamp(v.z, -1, 1));
	}

	inline Float sphericalPhi(const Vector3f& v) {
		Float p = std::atan2(v.y, v.x);
		return (p < 0) ? (p + 2 * Pi) : p;
	}

	inline Vector3f sphericalDirection(Float sinTheta, Float cosTheta, Float phi) {
		return Vector3f(sinTheta * std::cos(phi), sinTheta * std::sin(phi),
			cosTheta);
	}

	inline Vector3f sphericalDirection(Float sinTheta, Float cosTheta, Float phi,
		const Vector3f& x, const Vector3f& y,
		const Vector3f& z) {
		return sinTheta * std::cos(phi) * x + sinTheta * std::sin(phi) * y + cosTheta * z;
	}

	enum TransportMode 
	{
		Radiance,
		Importance
	};

	enum BxDFType 
	{
		BSDF_NONE = 0,
		BSDF_REFLECTION = 1 << 0,
		BSDF_TRANSMISSION = 1 << 1,
		BSDF_DIFFUSE = 1 << 2,
		BSDF_GLOSSY = 1 << 3,
		BSDF_SPECULAR = 1 << 4,
		BSDF_ALL = BSDF_DIFFUSE
		| BSDF_GLOSSY
		| BSDF_SPECULAR
		| BSDF_REFLECTION
		| BSDF_TRANSMISSION,
	};
}