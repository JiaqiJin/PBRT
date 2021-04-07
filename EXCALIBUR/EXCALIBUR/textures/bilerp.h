#pragma once

#include "../core/Header.h"
#include "../core/texture.h"

RENDERING_BEGIN

template <typename T>
class BilerpTexture : public Texture<T> {
public:
    BilerpTexture(std::unique_ptr<TextureMapping2D> mapping, const T& v00,
        const T& v01, const T& v10, const T& v11)
        : _mapping(std::move(mapping)), _v00(v00), _v01(v01), _v10(v10), _v11(v11) {

    }

    virtual T evaluate(const SurfaceInteraction& si) const override {
        Vector2f dstdx, dstdy;
        Point2f st = _mapping->map(si, &dstdx, &dstdy);
        return (1 - st[0]) * (1 - st[1]) * _v00
            + (1 - st[0]) * (st[1]) * _v01
            + (st[0]) * (1 - st[1]) * _v10
            + (st[0]) * (st[1]) * _v11;
    }

private:
    std::unique_ptr<TextureMapping2D> _mapping;
    const T _v00, _v01, _v10, _v11;
};

RENDERING_END