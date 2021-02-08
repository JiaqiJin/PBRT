﻿#include "material.hpp"
#include "primitive.hpp"
#include "texture.hpp"
#include "spectrum.hpp"
#include "bxdf.hpp"


PALADIN_BEGIN

// p'(u,v) = p(u,v) + d(u,v)n(u,v)
// 用人话来说就是p点经过bump贴图映射后的坐标为原始坐标加上bump偏移乘以法向量
// 这点还是非常容易理解的
// 但我们还忽略了一个点就是两个纹理坐标之间p是如何过渡的，其实就是该点uv方向的切线
void Material::bump(const std::shared_ptr<Texture<Float>>& d, SurfaceInteraction* si) {
	SurfaceInteraction siEval = *si;

	//todo 这里不是很理解为何要将两个方向相加
	// 取像素间隔的一半，所以乘以0.5
	Float du = 0.5f * (std::abs(si->dudx) + std::abs(si->dudy));
	if (du == 0) {
		du = 0.0005f;
	}
	// 计算u微小偏移之后pos的值
	siEval.pos = si->pos + du * si->shading.dpdu;
	// 新的uv坐标
	siEval.uv = si->uv + Vector2f(du, 0);
	// 由于法线可能被翻转，所以需要重新计算
	Normal3f tmpNormal = (Normal3f)cross(si->shading.dpdu, si->shading.dpdv);
	// 随u变化之后的法线方向
	siEval.normal = normalize(tmpNormal + du * si->dndu);
	// 计算经过u方向微小偏移之后的bump值
	Float uDisplace = d->evaluate(siEval);

	// 计算方式同u
	Float dv = 0.5f * (std::abs(si->dvdx) + std::abs(si->dvdy));
	if (dv == 0) {
		dv = 0.0005f;
	}
	siEval.pos = si->pos + dv * si->shading.dpdv;
	siEval.uv = si->uv + Vector2f(0.f, dv);
	siEval.normal = normalize(tmpNormal + dv * si->dndv);
	Float vDisplace = d->evaluate(siEval);

	// 偏移之前的bump值
	Float diplace = d->evaluate(*si);
	/**
	 * 计算微分几何信息，其实就是用这个表达式
	 *  dp'(u,v)	dp(u,v)	   db(u+△u,v) - db(u,v)                  dn(u,v)
	 * --------- ≈ --------- + --------------------- n(u,v) + b(u,v) ------
	 *     du		  du              △u                               du
	 */
	Vector3f dpdu = si->shading.dpdu
		+ (uDisplace - diplace) / du * Vector3f(si->shading.normal)
		+ diplace * Vector3f(si->shading.dndu);

	Vector3f dpdv = si->shading.dpdv
		+ (vDisplace - diplace) / dv * Vector3f(si->shading.normal)
		+ diplace * Vector3f(si->shading.dndv);

	si->setShadingGeometry(dpdu,
		dpdv,
		si->shading.dndu,
		si->shading.dndv,
		false);

}


PALADIN_END