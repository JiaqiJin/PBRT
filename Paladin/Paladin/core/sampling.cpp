﻿#include "sampling.hpp"

PALADIN_BEGIN

Vector3f uniformSampleHemisphere(const Point2f& u) {
    Float z = u[0];
    Float r = std::sqrt(std::max((Float)0, (Float)1. - z * z));
    Float phi = _2Pi * u[1];
    return Vector3f(r * std::cos(phi), r * std::sin(phi), z);
}

Vector3f uniformSampleSphere(const Point2f& u) {
    Float z = 1 - 2 * u[0];
    Float r = std::sqrt(std::max((Float)0, (Float)1 - z * z));
    Float phi = _2Pi * u[1];
    return Vector3f(r * std::cos(phi), r * std::sin(phi), z);
}

Vector3f uniformSamplePartialSphere(const Point2f& u, Float phiMax, Float cosThetaMin, Float cosThetaMax) {
    Float z = cosThetaMin - u[0] * (cosThetaMin - cosThetaMax);
    Float r = std::sqrt(std::max((Float)0, (Float)1 - z * z));
    Float phi = phiMax * u[1];
    return Vector3f(r * std::cos(phi), r * std::sin(phi), z);
}

Float uniformConePdf(Float cosThetaMax) {
    return 1 / (_2Pi * (1 - cosThetaMax));
}

Vector3f uniformSampleCone(const Point2f& u, Float cosThetaMax) {
    Float cosTheta = ((Float)1 - u[0]) + u[0] * cosThetaMax;
    Float sinTheta = std::sqrt((Float)1 - cosTheta * cosTheta);
    Float phi = u[1] * _2Pi;
    return Vector3f(std::cos(phi) * sinTheta, std::sin(phi) * sinTheta,
        cosTheta);
}

Point2f uniformSampleDisk(const Point2f& u) {
    Float r = std::sqrt(u[0]);
    Float theta = _2Pi * u[1];
    return Point2f(r * std::cos(theta), r * std::sin(theta));
}

Point2f concentricSampleDisk(const Point2f& u)
{
    // 把[0,1]映射到[-1,1]
    Point2f uOffset = 2.f * u - Vector2f(1, 1);

    // 退化到原点的情况
    if (uOffset.x == 0 && uOffset.y == 0) {
        return Point2f(0, 0);
    }

    /*
     r = x
    θ = y/x * π/4
    */
    Float theta, r;

    if (std::abs(uOffset.x) > std::abs(uOffset.y)) {
        // 如果x偏移量比y偏移量大
        r = uOffset.x;
        theta = PiOver4 * (uOffset.y / uOffset.x);
    }
    else {
        // 如果y偏移量比x偏移量大
        r = uOffset.y;
        theta = PiOver2 - PiOver4 * (uOffset.x / uOffset.y);
    }
    return r * Point2f(std::cos(theta), std::sin(theta));
}

Point2f uniformSampleSector(const Point2f& u, Float thetaMax) {
    Float r = std::sqrt(u[0]);
    Float theta = thetaMax * u[1];
    return Point2f(r * std::cos(theta), r * std::sin(theta));
}

Point2f uniformSamplePartialSector(const Point2f& u, Float thetaMax, Float rMin) {
    Float rMin2 = rMin * rMin;
    Float r = std::sqrt(u[0] * (1 - rMin2) + rMin2);
    Float theta = thetaMax * u[1];
    return Point2f(r * std::cos(theta), r * std::sin(theta));
}

Point2f uniformSampleTriangle(const Point2f& u) {
    Float su0 = std::sqrt(u[0]);
    return Point2f(1 - su0, u[1] * su0);
}

PALADIN_END