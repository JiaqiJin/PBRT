#ifndef macro_hpp
#define macro_hpp

#include "header.h"

#define CONSTEXPR constexpr

#define USING_KAWAII using namespace kawaii;
#define USING_STD using namespace std;

#define KAWAII_BEGIN namespace kawaii {
#define KAWAII_END }

#define DCHECK(args) assert(args)
#define CHECK_OP(op, a, b) DCHECK((a) op (b))
#define CHECK_NE(a, b) DCHECK((a) != (b))
#define CHECK_LE(a, b) DCHECK((a) <= (b))
#define CHECK_GE(a, b) DCHECK((a) >= (b))
#define CHECK_EQ(a, b) CHECK_OP(==, (a), (b))

#ifdef FLOAT_AS_DOUBLE
	typedef double Float;
#else
	typedef float Float;
#endif

#if defined(_MSC_VER)
#define KAWAII_HAVE_ALIGNED_MALLOC
#endif

#ifndef KAWAII_HAVE_ALIGNOF
#define KAWAII_HAVE_ALIGNOF 16
#endif

#define KAWAII_HAVE_POSIX_MEMALIGN

#define KAWAII_HAVE_CONSTEXPR

#ifndef KAWAII_L1_CACHE_LINE_SIZE
#define KAWAII_L1_CACHE_LINE_SIZE 64
#endif

#ifndef KAWAII_HAVE_ALIGNOF
#define KAWAII_HAVE_ALIGNOF 16
#endif

static CONSTEXPR Float MaxFloat = std::numeric_limits<Float>::max();
static CONSTEXPR Float Infinity = std::numeric_limits<Float>::infinity();
static CONSTEXPR Float MachineEpsilon = std::numeric_limits<Float>::epsilon() * 0.5;
static CONSTEXPR Float Epsilon = std::numeric_limits<Float>::epsilon();
static CONSTEXPR Float ShadowEpsilon = 0.0001f;
static CONSTEXPR Float Pi = 3.14159265358979323846;
static CONSTEXPR Float InvPi = 0.31830988618379067154;
static CONSTEXPR Float Inv2Pi = 0.15915494309189533577;
static CONSTEXPR Float Inv4Pi = 0.07957747154594766788;
static CONSTEXPR Float PiOver2 = 1.57079632679489661923;
static CONSTEXPR Float PiOver4 = 0.78539816339744830961;
static CONSTEXPR Float Sqrt2 = 1.41421356237309504880;

#endif /* macro_h */




