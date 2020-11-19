//
//  bvh.hpp
//  Paladin
#ifndef bvh_hpp
#define bvh_hpp

#include "header.h"
#include "primitive.hpp"

PALADIN_BEGIN

class BVHAccel : public Aggregate {

    enum class SplitMethod { SAH, HLBVH, Middle, EqualCounts };

};

PALADIN_END

#endif /* bvh_hpp */