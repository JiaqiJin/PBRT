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

RENDERING_END