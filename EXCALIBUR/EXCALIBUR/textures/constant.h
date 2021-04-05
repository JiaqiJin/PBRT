#pragma once

#include "../core/Header.h"
#include "../core/texture.h"

RENDERING_BEGIN

template<typename T>
class ConstantTexture : public Texture<T> {
public:
	ConstantTexture(const T &value) : _value(value) {}

	virtual T evaluate(const SurfaceInteraction&) const override {
		return _value;
	}

	static std::shared_ptr<ConstantTexture<T>> create(T v) {
		return std::make_shared<ConstantTexture<T>>(v);
	}

	static std::shared_ptr<ConstantTexture<Spectrum>> create(const Float rgb[3]) {
		return std::make_shared<ConstantTexture<Spectrum>>(Spectrum::FromRGB(rgb));
	}

private:
	T _value;
};

RENDERING_END