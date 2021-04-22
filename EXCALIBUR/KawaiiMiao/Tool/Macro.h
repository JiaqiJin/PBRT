#ifndef macro_h
#define macro_h

#include "../Core/Rendering.h"

#define CONSTEXPR constexpr

#define RENDERING_BEGIN namespace Rendering {
#define RENDERING_END }

#ifdef FLOAT_AS_DOUBLE
typedef double Float;
#else
typedef float Float;
#endif

typedef unsigned int uint;

#define rndm 0x100000000LL
#define rndc 0xB16
#define rnda 0x5DEECE66DLL

using Byte = unsigned char;

static CONSTEXPR Float MaxFloat = std::numeric_limits<Float>::max();
static CONSTEXPR Float Infinity = std::numeric_limits<Float>::infinity();
static CONSTEXPR Float MachineEpsilon = std::numeric_limits<Float>::epsilon() * 0.5;
static CONSTEXPR Float RayEpsilon = MachineEpsilon * 1500;
static CONSTEXPR Float ShadowEpsilon = RayEpsilon * 10;
static CONSTEXPR Float Epsilon = std::numeric_limits<Float>::epsilon();
//static CONSTEXPR Float ShadowEpsilon = 0.0001f;
static CONSTEXPR Float Pi = 3.14159265358979323846;
static CONSTEXPR Float _2Pi = 3.14159265358979323846 * 2;
static CONSTEXPR Float InvPi = 0.31830988618379067154;
static CONSTEXPR Float Inv2Pi = 0.15915494309189533577;
static CONSTEXPR Float Inv4Pi = 0.07957747154594766788;
static CONSTEXPR Float PiOver2 = 1.57079632679489661923;
static CONSTEXPR Float PiOver4 = 0.78539816339744830961;
static CONSTEXPR Float Sqrt2 = 1.41421356237309504880;

#endif