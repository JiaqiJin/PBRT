#ifndef constant_hpp
#define constant_hpp

#include "../core/header.h"
#include "../core/texture.hpp"

PALADIN_BEGIN

template<class T>
class ConstantTexture : public Texture<T> {
public:
    ConstantTexture(const T& value) : _value(value) {}

    virtual T evaluate(const SurfaceInteraction&) const {
        return _value;
    }
private:
	T _value;
};

PALADIN_END

#endif /* constant_hpp */
