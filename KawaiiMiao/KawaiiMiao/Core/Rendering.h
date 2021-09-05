#pragma once

#include <iostream>
#include <memory>
#include <limits>

#define GLM_FORCE_LEFT_HANDED
#include <glm/glm/glm.hpp>

#if defined(_MSC_VER)
#define NOMINMAX
#endif

#include "../Tool/Macro.h"
#include "../Tool/stringPrintf.h"
#include "../Tool/Logger.h"

#define ALLOCA(TYPE, COUNT) (TYPE *) alloca((COUNT) * sizeof(TYPE))

RENDER_BEGIN

inline Float lerp(Float t, Float v1, Float v2)
{
	return (1 - t) * v1 + t * v2;
}

inline Float gamma(int n)
{
	return (n * MachineEpsilon) / (1 - n * MachineEpsilon);
}

inline Float gammaCorrect(Float value)
{
	if (value <= 0.0031308f)
		return 12.92f * value;
	return 1.055f * glm::pow(value, (Float)(1.f / 2.4f)) - 0.055f;
}

inline Float inverseGammaCorrect(Float value)
{
	if (value <= 0.04045f)
		return value * 1.f / 12.92f;
	return glm::pow((value + 0.055f) * 1.f / 1.055f, (Float)2.4f);
}


template <typename T, typename U, typename V>
inline T clamp(T val, U low, V high)
{
	if (val < low)
		return low;
	else if (val > high)
		return high;
	else
		return val;
}

class Ray;
class Interaction;
class SurfaceInteraction;
class Film;
class BSDF;
class BxDF;
class Light;
class Shape;
class Scene;
class Camera;
class Primitive;
class Medium;
class FilmTile;
class Sampler;
class Material; 
class AreaLight;
class Transform;
class Integrator;
class CameraSample;
class RGBSpectrum;
class Distribution1D;
class VisibilityTester;
class MemoryArena;
class MediumInteraction;
struct MediumInterface;

using Spectrum = RGBSpectrum;


// TransportMode Declarations
enum class TransportMode { Radiance, Importance };

inline uint32_t floatToBits(float f)
{
	uint32_t ui;
	memcpy(&ui, &f, sizeof(float));
	return ui;
}

inline float bitsToFloat(uint32_t ui)
{
	float f;
	memcpy(&f, &ui, sizeof(uint32_t));
	return f;
}

inline uint64_t floatToBits(double f)
{
	uint64_t ui;
	memcpy(&ui, &f, sizeof(double));
	return ui;
}

inline double bitsToFloat(uint64_t ui)
{
	double f;
	memcpy(&f, &ui, sizeof(uint64_t));
	return f;
}

RENDER_END