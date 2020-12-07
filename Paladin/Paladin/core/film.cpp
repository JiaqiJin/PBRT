#include "film.hpp"

PALADIN_BEGIN

Film::Film(const Point2i& resolution, const AABB2f& cropWindow,
    std::unique_ptr<Filter> filt, Float diagonal,
    const std::string& filename, Float scale)
    :fullResolution(resolution), diagonal(diagonal * .001),
    filter(std::move(filt)), filename(filename), scale(scale)
{
    // upper-left to lower right corner of crowp window pixels bounds
    croppedPixelBounds =
        AABB2i(Point2i(std::ceil(fullResolution.x * cropWindow.pMin.x),
            std::ceil(fullResolution.y * cropWindow.pMin.y)),
            Point2i(std::ceil(fullResolution.x * cropWindow.pMax.x),
                std::ceil(fullResolution.y * cropWindow.pMax.y)));

    pixels = std::unique_ptr<Pixel[]>(new Pixel[croppedPixelBounds.Area()]);
    //f(x,y) = f(|x|,||y) hold value for positives quadrant of filter offset
    int offset = 0;
    for (int y = 0; y < filterTableWidth; ++y) {
        for (int x = 0; x < filterTableWidth; ++x, ++offset) {
            Point2f p;
            p.x = (x + 0.5f) * filter->radius.x / filterTableWidth;
            p.y = (y + 0.5f) * filter->radius.y / filterTableWidth;
            filterTable[offset] = filter->evaluate(p);
        }
    }
}

/*
Return the area to be sampled. 
Pixels reconstruction generates image sample a bit outside of the range.
Pixels at boundary of the image will have an equal density of sample around them in all dir.
Crop window eliminate the artifacts at edge of the subimages.

Computing the  sample bounds involve the half-pixels offsets when convert from discret to continuos pixels coord.
expanding by the filter radius, and then rounding outward.
*/
AABB2i Film::GetSampleBounds() const
{
    AABB2f floatBounds(floor(Point2f(croppedPixelBounds.pMin) + Vector2f(0.5f , 0.5f))- filter->radius,
                        ceil(Point2f(croppedPixelBounds.pMax) - Vector2f(0.5f,0.5f) + filter->radius));
    return (AABB2i)floatBounds;
}

/*
Giving the film diagonal and aspect radius of the image, compute size of sensor in xand y dir for Realiztic camera.
x = √ (d^2 / 1+a^2)
a = aspect ratio, d = diagonale length
a = y / x;
*/
AABB2f Film::GetPhysicalExtent() const
{
    Float aspect = (Float)fullResolution.y / (Float)fullResolution.x;
    Float x = std::sqrt(diagonal * diagonal / (1 + aspect * aspect));
    Float y = aspect * x;
    return AABB2f(Point2f(-x / 2, -y / 2), Point2f(x / 2, y / 2));
}

PALADIN_END