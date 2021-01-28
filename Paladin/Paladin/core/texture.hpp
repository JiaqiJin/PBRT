#ifndef texture_hpp
#define texture_hpp

#include "header.h"
#include "interaction.hpp"

PALADIN_BEGIN

template<typename T>
class Texture {
public:
    virtual T evaluate(const SurfaceInteraction&) = 0;

    virtual ~Texture() {
    }
};

PALADIN_END

#endif /* texture_hpp */
