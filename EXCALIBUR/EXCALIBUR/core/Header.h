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

#ifdef PALADIN_SAMPLED_SPECTRUM
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

#include "../math/mathutil.h"

#include "../math/vector.h"
#include "../math/point.h"
#include "../math/ray.h"
#include "../math/bounds.h"
#include "../tools/memory.h"
#include "../tools/errfloat.h"
#include "spectrum.h"

namespace Rendering 
{
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