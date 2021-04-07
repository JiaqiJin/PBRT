#include "MicrofacetDistribution.h"
#include "../BxDF.h"

RENDERING_BEGIN

Float MicrofacetDistribution::pdfDir(const Vector3f& wo, const Vector3f& wh) const {
    if (_sampleVisibleArea) {
        // 如果只计算wo视角可见部分，则需要乘以史密斯遮挡函数再归一化
        // 归一化方式如下，利用以上3式
        // cosθo = ∫[hemisphere]G1(ωo,ωh) max(0, ωo · ωh) D(ωh)dωh
        // 将cos项移到右边，得
        // 1 = ∫[hemisphere]G1(ωo,ωh) max(0, ωo · ωh) D(ωh) / (cosθo) dωh
        // 则概率密度函数为 G1(ωo,ωh) max(0, ωo · ωh) D(ωh) / (cosθo) ,代码如下
        return D(wh) * G1(wo) * absDot(wo, wh) / AbsCosTheta(wo);
    }
    else {
        // 如果忽略几何遮挡，则概率密度函数值就是D(ωh) * cosθh
        return D(wh) * AbsCosTheta(wh);
    }
}

Float BeckmannDistribution::D(const Vector3f& wh) const {
    Float tan2Theta = tanTheta2(wh);
    if (std::isinf(tan2Theta)) return 0.;
    Float cos4Theta = cos2Theta(wh) * cos2Theta(wh);
    return std::exp(-tan2Theta * (cos2Phi(wh) / (_alphax * _alphax) +
        sin2Phi(wh) / (_alphay * _alphay))) /
        (Pi * _alphax * _alphay * cos4Theta);
}

Float BeckmannDistribution::Lambda(const Vector3f& w) const {
    Float absTanTheta = std::abs(tanTheta(w));
    if (std::isinf(absTanTheta)) {
        // 当θ为90°时，会出现tan值无穷大的情况，为了避免这种异常发生
        // 我们返回0
        return 0.;
    }
    // α^2 = (cosθh)^2/αx^2 + (sinθh)^2/αy^2)
    Float alpha = std::sqrt(cos2Phi(w) * _alphax * _alphax + sin2Phi(w) * _alphay * _alphay);
    Float a = 1 / (alpha * absTanTheta);
    if (a >= 1.6f) {
        return 0;
    }
    return (1 - 1.259f * a + 0.396f * a * a) / (3.535f * a + 2.181f * a * a);
}

Vector3f BeckmannDistribution::sample_wh(const Vector3f& wo,
    const Point2f& u) const {
    // TODO
    return Vector3f(0.f,0.f,0.f);
}

// TrowbridgeReitzDistribution

Float TrowbridgeReitzDistribution::D(const Vector3f& wh) const {
    Float _cosTheta = absCosTheta(wh);
    if (_cosTheta == 1) {
        // 当θ为90°时，会出现tan值无穷大的情况，为了避免这种异常发生
        // 我们返回0
        return 0.;
    }
    Float cosTheta4 = (_cosTheta * _cosTheta) * (_cosTheta * _cosTheta);
    Float _tanTheta2 = tanTheta2(wh);

    Float sinPhi2 = sin2Phi(wh);
    Float cosPhi2 = cos2Phi(wh);

    Float alphax2 = _alphax * _alphax;
    Float alphay2 = _alphay * _alphay;

    Float term1 = 1 + (cosPhi2 / alphax2 + sinPhi2 / alphay2) * _tanTheta2;
    Float term2 = term1 * term1;
    return 1.0 / (Pi * _alphax * _alphay * cosTheta4 * term2);
}

static void TrowbridgeReitzSample11(Float cosTheta, Float U1, Float U2,
    Float* slope_x, Float* slope_y) {
    if (cosTheta > .9999) {
        Float r = std::sqrt(U1 / (1 - U1));
        Float phi = 6.28318530718 * U2;
        *slope_x = r * cos(phi);
        *slope_y = r * sin(phi);
        return;
    }

    Float sinTheta =
        std::sqrt(std::max((Float)0, (Float)1 - cosTheta * cosTheta));
    Float tanTheta = sinTheta / cosTheta;
    Float a = 1 / tanTheta;
    Float G1 = 2 / (1 + std::sqrt(1.f + 1.f / (a * a)));

    Float A = 2 * U1 / G1 - 1;
    Float tmp = 1.f / (A * A - 1.f);
    if (tmp > 1e10) tmp = 1e10;
    Float B = tanTheta;
    Float D = std::sqrt(
        std::max(Float(B * B * tmp * tmp - (A * A - B * B) * tmp), Float(0)));
    Float slope_x_1 = B * tmp - D;
    Float slope_x_2 = B * tmp + D;
    *slope_x = (A < 0 || slope_x_2 > 1.f / tanTheta) ? slope_x_1 : slope_x_2;

    Float S;
    if (U2 > 0.5f) {
        S = 1.f;
        U2 = 2.f * (U2 - .5f);
    }
    else {
        S = -1.f;
        U2 = 2.f * (.5f - U2);
    }
    Float z =
        (U2 * (U2 * (U2 * 0.27385f - 0.73369f) + 0.46341f)) /
        (U2 * (U2 * (U2 * 0.093073f + 0.309420f) - 1.000000f) + 0.597999f);
    *slope_y = S * z * std::sqrt(1.f + *slope_x * *slope_x);

    DCHECK(!std::isinf(*slope_y));
    DCHECK(!std::isnan(*slope_y));
}

static Vector3f TrowbridgeReitzSample(const Vector3f& wi, Float alpha_x,
    Float alpha_y, Float U1, Float U2) {
    Vector3f wiStretched =
        normalize(Vector3f(alpha_x * wi.x, alpha_y * wi.y, wi.z));

    Float slope_x, slope_y;
    TrowbridgeReitzSample11(cosTheta(wiStretched), U1, U2, &slope_x, &slope_y);

    Float tmp = cosPhi(wiStretched) * slope_x - sinPhi(wiStretched) * slope_y;
    slope_y = sinPhi(wiStretched) * slope_x + cosPhi(wiStretched) * slope_y;
    slope_x = tmp;

    slope_x = alpha_x * slope_x;
    slope_y = alpha_y * slope_y;

    return normalize(Vector3f(-slope_x, -slope_y, 1.));
}

Float TrowbridgeReitzDistribution::Lambda(const Vector3f& w) const {
    Float absTanTheta = std::abs(tanTheta(w));
    if (std::isinf(absTanTheta)) {
        // 当θ为90°时，会出现tan值无穷大的情况，为了避免这种异常发生
        // 我们返回0
        return 0.;
    }
    // α^2 = (cosθh)^2/αx^2 + (sinθh)^2/αy^2)
    Float alpha = std::sqrt(cos2Phi(w) * _alphax * _alphax + sin2Phi(w) * _alphay * _alphay);
    Float alpha2Tan2Theta = (alpha * absTanTheta) * (alpha * absTanTheta);
    return (-1 + std::sqrt(1.f + alpha2Tan2Theta)) / 2;
}

Vector3f TrowbridgeReitzDistribution::sample_wh(const Vector3f& wo,
    const Point2f& u) const {
    Vector3f wh;
    if (_sampleVisibleArea) {
        // 不忽略几何遮挡
        // 这个不忽略阻挡的采样太TMD复杂了，暂时不管推导过程了，暂时认怂，搞完主线再说todo        
        bool flip = wo.z < 0;
        wh = TrowbridgeReitzSample(flip ? -wo : wo, _alphax, _alphay, u[0], u[1]);
        if (flip) {
            wh = -wh;
        }
    }
    else {
        Float cosTheta = 0, phi = (2 * Pi) * u[1];
        if (_alphax == _alphay) {
            // 计算各向同性
            Float tanTheta2 = _alphax * _alphax * u[0] / (1.0f - u[0]);
            cosTheta = 1 / std::sqrt(1 + tanTheta2);
        }
        else {
            // 计算各向异性
            phi = std::atan(_alphay / _alphax * std::tan(2 * Pi * u[1] + .5f * Pi));
            if (u[1] > .5f) {
                phi += Pi;
            }
            Float sinPhi = std::sin(phi), cosPhi = std::cos(phi);
            const Float alphax2 = _alphax * _alphax, alphay2 = _alphay * _alphay;
            const Float alpha2 = 1 / (cosPhi * cosPhi / alphax2 + sinPhi * sinPhi / alphay2);
            Float tanTheta2 = alpha2 * u[0] / (1 - u[0]);
            cosTheta = 1 / std::sqrt(1 + tanTheta2);
        }
        Float sinTheta = std::sqrt(std::max((Float)0., (Float)1. - cosTheta * cosTheta));
        wh = sphericalDirection(sinTheta, cosTheta, phi);
        if (!sameHemisphere(wo, wh)) {
            wh = -wh;
        }
    }
    return wh;
}

RENDERING_END