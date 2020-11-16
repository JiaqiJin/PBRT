#include "sampling.hpp"

PALADIN_BEGIN

Vector3f uniformSampleHemisphere(const Point2f &u) {
    Float z = u[0];
    Float r = std::sqrt(std::max((Float)0, (Float)1. - z * z));
    Float phi = 2 * Pi * u[1];
    return Vector3f(r * std::cos(phi), r * std::sin(phi), z);
}

Vector3f uniformSampleSphere(const Point2f &u) {
    Float z = 1 - 2 * u[0];
    Float r = std::sqrt(std::max((Float)0, (Float)1 - z * z));
    Float phi = 2 * Pi * u[1];
    return Vector3f(r * std::cos(phi), r * std::sin(phi), z);
}

Float uniformConePdf(Float cosThetaMax) {
    return 1 / (2 * Pi * (1 - cosThetaMax));
}

Vector3f uniformSampleCone(const Point2f &u, Float cosThetaMax) {
    Float cosTheta = ((Float)1 - u[0]) + u[0] * cosThetaMax;
    Float sinTheta = std::sqrt((Float)1 - cosTheta * cosTheta);
    Float phi = u[1] * 2 * Pi;
    return Vector3f(std::cos(phi) * sinTheta, std::sin(phi) * sinTheta,
                    cosTheta);
}

PALADIN_END
