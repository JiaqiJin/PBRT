#ifndef macro_h
#define macro_h

#include "../core/Header.h"

#define CONSTEXPR constexpr

//#define OUTPUT_LINENO 1

#ifdef OUTPUT_LINENO
#define COUT std::cout << __FILE__ << " " << __LINE__ << std::endl
#else
#define COUT std::cout
#endif

#define TINYOBJLOADER_IMPLEMENTATION

#define USING_RENDERING using namespace Rendering;
#define USING_STD using namespace std;

#define RENDERING_BEGIN namespace Rendering {
#define RENDERING_END }

#define DCHECK(args) assert(args)
#define CHECK_OP(op, a, b) DCHECK((a) op (b))
#define CHECK_NE(a, b) DCHECK((a) != (b))
#define CHECK_LE(a, b) DCHECK((a) <= (b))
#define CHECK_LT(a, b) DCHECK((a) < (b))
#define CHECK_GE(a, b) DCHECK((a) >= (b))
#define CHECK_GT(a, b) DCHECK((a) > (b))
#define CHECK_EQ(a, b) CHECK_OP(==, (a), (b))

#ifdef FLOAT_AS_DOUBLE
typedef double Float;
#else
typedef float Float;
#endif

#define RENDERING_HAVE_CONSTEXPR

#ifndef RENDERING_HAVE_ALIGNOF
#define RENDERING_HAVE_ALIGNOF 16
#endif

#define RENDERING_HAVE_HEX_FP_CONSTANTS

#ifndef RENDERING_HAVE_HEX_FP_CONSTANTS
static const double DoubleOneMinusEpsilon = 0.99999999999999989;
static const float FloatOneMinusEpsilon = 0.99999994;
#elif _MSC_VER
static const double DoubleOneMinusEpsilon = 0.99999999999999989;
static const float FloatOneMinusEpsilon = 0.99999994;
#else
static const double DoubleOneMinusEpsilon = 0x1.fffffffffffffp-1;
static const float FloatOneMinusEpsilon = 0x1.fffffep-1;
#endif

#ifdef RENDERING_FLOAT_AS_DOUBLE
static const Float OneMinusEpsilon = DoubleOneMinusEpsilon;
#else
static const Float OneMinusEpsilon = FloatOneMinusEpsilon;
#endif

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

#define THREAD_LOCAL thread_local

#define NotImplementedError(args) \
    throw std::runtime_error(StringPrintf("function %s not implemented\n", args));

#define RENDERING_HAVE_ITIMER 1

#ifdef RENDERING_HAVE_ITIMER
#define TRY_PROFILE(args) ProfilePhase __pp(args);
#define PROFILE(args, arg2) ProfilePhase arg2(args);
#else
#define TRY_PROFILE(args) ;
#define PROFILE(arg1, arg2) ;
#endif

#endif /* macro_h */



