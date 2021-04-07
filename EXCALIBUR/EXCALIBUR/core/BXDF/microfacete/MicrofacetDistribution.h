#pragma once

#include "../../Header.h"
#include "../../spectrum.h"

RENDERING_BEGIN

class MicrofacetDistribution {
public:
	virtual ~MicrofacetDistribution() {}

	virtual Float D(const Vector3f& wh) const = 0;

	virtual Float Lambda(const Vector3f& w) const = 0;

	Float G1(const Vector3f& w) const {
		//    if (Dot(w, wh) * CosTheta(w) < 0.) return 0.;
		return 1 / (1 + Lambda(w));
	}
	virtual Float G(const Vector3f& wo, const Vector3f& wi) const {
		return 1 / (1 + Lambda(wo) + Lambda(wi));
	}

	virtual Vector3f sample_wh(const Vector3f& wo, const Point2f& u) const = 0;

	Float pdfDir(const Vector3f& wo, const Vector3f& wh) const;

	virtual std::string toString() const = 0;
protected:
	MicrofacetDistribution(bool sampleVisibleArea)
		: _sampleVisibleArea(sampleVisibleArea) {

	}
	// 是否只采样可见区域，如果为真，则加入几何遮挡，否则忽略几何遮挡
	const bool _sampleVisibleArea;
};

class BeckmannDistribution : public MicrofacetDistribution {
public:
	/**             e^(-(tanθh)^2 / α^2)
	* D(ωh) = ----------------------------
	*                π α^2 (cosθh)^4 */
	virtual Float D(const Vector3f& wh) const;

	virtual Vector3f sample_wh(const Vector3f& wo, const Point2f& u) const;

	virtual std::string toString() const {
		return StringPrintf("[ BeckmannDistribution alphax: %f alphay: %f ]",
			_alphax, _alphay);
	}

	BeckmannDistribution(Float alphax, Float alphay, bool samplevis = true)
		: MicrofacetDistribution(samplevis), _alphax(alphax), _alphay(alphay) {
	}

private:
	virtual Float Lambda(const Vector3f& w) const;

	const Float _alphax, _alphay;
};

class TrowbridgeReitzDistribution : public MicrofacetDistribution{
public:
    /**
     * 粗糙度转α参数，代码直接照搬pbrt
     * @param  roughness 粗糙度
     * @return           α值
     */
    static inline Float RoughnessToAlpha(Float roughness) {
        roughness = std::max(roughness, (Float)1e-3);
        Float x = std::log(roughness);
        return 1.62142f + 0.819955f * x + 0.1734f * x * x + 0.0171201f * x * x * x +
        0.000640711f * x * x * x * x;
    }

    TrowbridgeReitzDistribution(Float alphax, Float alphay,
                                bool samplevis = true)
    : MicrofacetDistribution(samplevis),
    _alphax(alphax),
    _alphay(alphay) {

    }

    /*
     * 法线分布函数
     *                                             1
     * D(ωh) = -----------------------------------------------------------------------------
     *             π αx αy (cosθh)^4 [1 + (tanθh)^2 ((cosθh)^2/αx^2 + (sinθh)^2/αy^2)]^2
    */
    virtual Float D(const Vector3f & wh) const;

    virtual Vector3f sample_wh(const Vector3f& wo, const Point2f& u) const;

    virtual std::string toString() const {
        return StringPrintf("[ TrowbridgeReitzDistribution alphax: %f alphay: %f ]",
                            _alphax, _alphay);
    }

private:
    virtual Float Lambda(const Vector3f& w) const;

    // todo 这里也是可以优化的
    const Float _alphax, _alphay;
};


RENDERING_END