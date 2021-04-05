#pragma once

#include "../core/Header.h"
#include "../core/texture.h"

RENDERING_BEGIN

// 用于两个纹理相乘的情况
template <typename T1, typename T2>
class ScaleTexture : public Texture<T2> {
public:
    ScaleTexture(const std::shared_ptr<Texture<T1>>& tex1,
        const std::shared_ptr<Texture<T2>>& tex2)
        : _tex1(tex1), _tex2(tex2) { }

    virtual T2 evaluate(const SurfaceInteraction& si) const override {
        return _tex1->evaluate(si) * _tex2->evaluate(si);
    }
private:
	std::shared_ptr<Texture<T1>> _tex1;
	std::shared_ptr<Texture<T2>> _tex2;
};

RENDERING_END