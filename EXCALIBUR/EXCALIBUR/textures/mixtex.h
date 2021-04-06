#pragma once

#include "../core/Header.h"
#include "../core/texture.h"

RENDERING_BEGIN

template<typename T>
class MixTexture : public Texture<T> {
public:
    MixTexture(const std::shared_ptr<Texture<T>>& tex1,
        const std::shared_ptr<Texture<T>>& tex2,
        const std::shared_ptr<Texture<Float>>& amount)
        : _tex1(tex1), _tex2(tex2), _amount(amount) {

    }
    virtual T evaluate(const SurfaceInteraction& si) const override {
        T t1 = _tex1->evaluate(si), t2 = _tex2->evaluate(si);
        Float amt = _amount->evaluate(si);
        return (1 - amt) * t1 + amt * t2;
    }

private:
    std::shared_ptr<Texture<T>> _tex1, _tex2;
    std::shared_ptr<Texture<Float>> _amount;
};

RENDERING_END