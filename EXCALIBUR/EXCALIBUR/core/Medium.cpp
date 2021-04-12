﻿#include "Medium.h"

RENDERING_BEGIN

Float HenyeyGreenstein::p(const Vector3f& wo, const Vector3f& wi) const {
    return phaseHG(dot(wo, wi), _g);
}

Float HenyeyGreenstein::sample_p(const Vector3f& wo, Vector3f* wi, const Point2f& u) const {
    Float cosTheta = 0;
    if (std::abs(_g) < 1e-3) {
        // 如果各向异性系数很小，则当做各向同性处理 其实就是均匀采样球面
        cosTheta = 1 - 2 * u[0];
    }
    else {
        //         1                    1 - g^2
        // cosθ = ---- [1 + g^2 - (-----------------)^2]
        //         2π                1 - g + 2 ξ g
        Float sqrTerm = (1 - _g * _g) / (1 - _g + 2 * _g * u[0]);
        cosTheta = (1 + _g * _g - sqrTerm * sqrTerm) / (2 * _g);
    }
    Float sinTheta = std::sqrt(std::max((Float)0, 1 - cosTheta * cosTheta));

    Float phi = 2 * Pi * u[1];
    Vector3f v1, v2;
    // 构造坐标系
    coordinateSystem(wo, &v1, &v2);
    // 根据指定坐标系以及方位角天顶角生成向量
    *wi = sphericalDirection(sinTheta, cosTheta, phi, v1, v2, -wo);
    return phaseHG(-cosTheta, _g);
}

RENDERING_END