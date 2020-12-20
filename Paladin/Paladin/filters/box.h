﻿#ifndef box_hpp
#define box_hpp

#include "filter.h"

PALADIN_BEGIN

/**
 * 盒式滤波器，效果最差的滤波器
 * 频域下为sinc函数
 */
class BoxFilter : public Filter
{
public:
	BoxFilter(const Vector2f& radius) : Filter(radius){
	}

    /**
     * 由于外层传入值的时候已经做了范围校验，可以直接返回1
     */
    virtual Float evaluate(const Point2f& p) const {
        return 1.0f;
    }
};

PALADIN_END

#endif
