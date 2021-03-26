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

RENDERING_END