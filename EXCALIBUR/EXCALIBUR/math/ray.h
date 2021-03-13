#ifndef ray_h
#define ray_h

#include "../core/Header.h"

RENDERING_BEGIN

class Ray
{
public:

public:
    // 起点
    Point3f ori;
    // 方向，单位向量
    Vector3f dir;
    // 光线的最远距离
    mutable Float tMax;
    // 发射的时间，用于做motion blur
    Float time;
    // 光线所在的介质(水，空气，玻璃等)
    const Medium* medium;

    mutable Float tMin;
};

RENDERING_END

#endif /* ray_h */