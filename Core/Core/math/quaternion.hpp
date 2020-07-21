#ifndef quaternion_hpp
#define quaternion_hpp

#include "header.h"

KAWAII_BEGIN

struct Quaternion {
	Quaternion() : _v(0, 0, 0), _w(1) {}

	Vector3f _v;
	Float _w;
};

KAWAII_END

#endif /* quaternion_hpp */
