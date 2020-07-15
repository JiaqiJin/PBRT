#ifndef header_h
#define header_h

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
class Bounds3;

template<typename T>
class Bounds2;

template<typename T>
class Direction3;

class Medium;

class Ray;

class Transform;

#include<iostream>
#include<cmath>
#include <algorithm>
#include <limits>
#include <assert.h>

#include "../util/macro.h"
#include "../util/stringprint.h"

#include "../math/vector.h"
#include "../math/point.h"
#include "../math/bound.h"
#include "../math/mathutil.h"
#include "../math/ray.h"
#include "../math/transform.hpp"

KAWAII_BEGIN

KAWAII_END

#endif /* header_h */
