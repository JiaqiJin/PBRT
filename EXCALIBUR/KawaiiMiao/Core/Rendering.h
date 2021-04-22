#pragma once

#include <memory>
#include <limits>

#define GLM_FORCE_LEFT_HANDED
#include <glm/glm/glm.hpp>

#include "../Tool/Macro.h"
#include "../Math/KMathUtil.h"

RENDERING_BEGIN

inline Float lerp(Float t, Float v1, Float v2)
{
	return (1 - t) * v1 + t * v2;
}

inline Float gamma(int n)
{
	return (n * MachineEpsilon) / (1 - n * MachineEpsilon);
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

RENDERING_END