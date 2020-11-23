#ifndef film_hpp
#define film_hpp

#include "header.h"
#include "spectrum.hpp"
#include "filter.h"

PALADIN_BEGIN

struct FilmTilePixel {
    Spectrum contribSum = 0.f;
    Float filterWeightSum = 0.f;
};

class Film
{
public:
    Film();

private:

};

PALADIN_END

#endif /* film_hpp */