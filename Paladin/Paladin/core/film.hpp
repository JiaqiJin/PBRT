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
    Film(const Point2i& resolution, const AABB2f& cropWindow,
        std::unique_ptr<Filter> filt, Float diagonal,
        const std::string& filename, Float scale);

    AABB2i GetSampleBounds() const;
    AABB2f GetPhysicalExtent() const;

    const Point2f fullResolution;
    const Float diagonal;
    std::unique_ptr<Filter> filter;
    const std::string filename;
    AABB2i croppedPixelBounds;

private:
    
    struct Pixel
    {
        Float xyz[3] = { 0,0,0 };
        Float filterWeightSum = 0; // sample contribution to the pixels
        //AtomicFloat splatXYZ[3];
        Float pad; // 32 bytes
    };
    std::unique_ptr<Pixel[]> pixels;
    static constexpr int filterTableWidth = 16;
    //Film class precalculate a table of filter values(16 o more pixels per image samples)
    Float filterTable[filterTableWidth * filterTableWidth];
    const Float scale;
};


class FilmTile
{
public:

};

PALADIN_END

#endif /* film_hpp */