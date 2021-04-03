#include "BSDF.h"

RENDERING_BEGIN

// BSDF
BSDF::BSDF(const SurfaceInteraction& si, Float eta)
	: eta(eta),
	_gNormal(si.normal),
	_sNormal(si.shading.normal),
	_sTangent(normalize(si.shading.dpdu)),
	_tTangent(cross(_sNormal, _sTangent)) {

}

BSDF::BSDF(Float eta)
	: eta(eta) {

}

void BSDF::updateGeometry(const SurfaceInteraction& si) {
	_gNormal = si.normal;
	_sNormal = si.shading.normal;
	_sTangent = normalize(si.shading.dpdu);
	_tTangent = cross(_sNormal, _sTangent);
}

Spectrum BSDF::f(const Vector3f& woW, const Vector3f& wiW, BxDFType flags) const {
	// TRY_PROFILE
	Vector3f wi = worldToLocal(wiW);
	Vector3f wo = worldToLocal(woW);
	if (wo.z == 0) {
		return 0.;
	}
	bool reflect = dot(wiW, _gNormal) * dot(woW, _gNormal) > 0;
	Spectrum f(0.0f);
	for (int i = 0; i < nBxDFs; ++i) {
		if (bxdfs[i]->MatchesFlags(flags) &&
			((reflect && (bxdfs[i]->type & BSDF_REFLECTION)) ||
				(!reflect && (bxdfs[i]->type & BSDF_TRANSMISSION)))) {
			f += bxdfs[i]->f(wo, wi);
		}
	}
	return f;
}

Spectrum BSDF::rho_hh(int nSamples, const Point2f* samples1,
	const Point2f* samples2, BxDFType flags) const {
	Spectrum ret(0.f);
	for (int i = 0; i < nBxDFs; ++i) {
		if (bxdfs[i]->MatchesFlags(flags)) {
			ret += bxdfs[i]->rho_hh(nSamples, samples1, samples2);
		}
	}
	return ret;
}

Spectrum BSDF::rho_hd(const Vector3f& wo, int nSamples, const Point2f* samples,
	BxDFType flags) const {
	Spectrum ret(0.f);
	for (int i = 0; i < nBxDFs; ++i) {
		if (bxdfs[i]->MatchesFlags(flags)) {
			ret += bxdfs[i]->rho_hd(wo, nSamples, samples);
		}
	}
	return ret;
}

Spectrum BSDF::sample_f(const Vector3f& woWorld, Vector3f* wiWorld,
	const Point2f& u, Float* pdf, BxDFType type,
	BxDFType* sampledType) const {
	return Spectrum(0.f);
}

Float BSDF::pdfDir(const Vector3f& woWorld, const Vector3f& wiWorld,
	BxDFType flags) const {
	return 1.0f;
}

std::string BSDF::toString() const {
	std::string s = StringPrintf("[ BSDF eta: %f nBxDFs: %d", eta, nBxDFs);
	for (int i = 0; i < nBxDFs; ++i)
		s += StringPrintf("\n  bxdfs[%d]: ", i) + bxdfs[i]->toString();
	return s + std::string(" ]");
}

RENDERING_END