#include "film.h"
#include "../tools/fileio.h"

RENDERING_BEGIN

Film::Film(const Point2i& resolution, const AABB2i& cropWindow,
    std::unique_ptr<Filter> filt, Float diagonal,
    const std::string& filename, Float scale, Float maxSampleLuminance)
    : fullResolution(resolution),
    diagonal(diagonal * .001),
    filter(std::move(filt)),
    filename(filename),
    scale(scale),
    maxSampleLuminance(maxSampleLuminance) {
    croppedPixelBounds = 
        AABB2i(Point2i(std::ceil(fullResolution.x * cropWindow.pMin.x), std::ceil(fullResolution.y * cropWindow.pMin.y)),
        Point2i(std::ceil(fullResolution.x * cropWindow.pMax.x), std::ceil(fullResolution.y * cropWindow.pMax.y)));

    pixels = std::unique_ptr<Pixel[]>(new Pixel[croppedPixelBounds.area()]);

    int offset = 0;
    for (int y = 0; y < filterTableWidth; ++y) {
        for (int x = 0; x < filterTableWidth; ++x, ++offset) {
            Point2f p;
            p.x = (x + 0.5f) * filter->radius.x / filterTableWidth;
            p.y = (y + 0.5f) * filter->radius.y / filterTableWidth;
            filterTable[offset] = filter->Evaluate(p);
        }
    }
}

AABB2i Film::GetSampleBounds() const {
    AABB2f floatBounds(
        Rendering::floor(Point2f(croppedPixelBounds.pMin) + Vector2f(0.5f, 0.5f) -
            filter->radius),
        Rendering::ceil(Point2f(croppedPixelBounds.pMax) - Vector2f(0.5f, 0.5f) +
            filter->radius));
    return (AABB2i)floatBounds;
}

AABB2f Film::GetPhysicalExtent() const {
    Float aspect = (Float)fullResolution.y / (Float)fullResolution.x;
    Float x = std::sqrt(diagonal * diagonal / (1 + aspect * aspect));
    Float y = aspect * x;
    return AABB2f(Point2f(-x / 2, -y / 2), Point2f(x / 2, y / 2));
}

std::unique_ptr<FilmTile> Film::GetFilmTile(const AABB2i& sampleBounds) {
    Vector2f halfPixel = Vector2f(0.5f, 0.5f);
    AABB2f floatBounds = (AABB2f)sampleBounds;
    Point2i p0 = (Point2i)Rendering::ceil(floatBounds.pMin - halfPixel -
        filter->radius);
    Point2i p1 = (Point2i)Rendering::floor(floatBounds.pMax - halfPixel +
        filter->radius) + Point2i(1, 1);

    AABB2i tilePixelBounds = intersect(AABB2i(p0, p1), croppedPixelBounds);

    return std::unique_ptr<FilmTile>(new FilmTile(tilePixelBounds,
        filter->radius, filterTable, filterTableWidth));
}

void Film::MergeFilmTile(std::unique_ptr<FilmTile> tile) {
    INFO("Merging film tile", tile->pixelBounds);
    std::lock_guard<std::mutex> lock(mutex);
    for (Point2i pixel : tile->GetPixelBounds()) {
        // Merge _pixel_ into _Film::pixels_
        const FilmTilePixel& tilePixel = tile->GetPixel(pixel);
        Pixel& mergePixel = GetPixel(pixel);
        float xyz[3];
        tilePixel.contribSum.ToXYZ(xyz);
        for (int i = 0; i < 3; i++) mergePixel.xyz[i] += xyz[i];
        mergePixel.filterWeightSum += tilePixel.filterWeightSum;
    }
}

void Film::SetImage(const Spectrum* img) const {
    int nPixels = croppedPixelBounds.area();
    for (int i = 0; i < nPixels; ++i) {
        Pixel& p = pixels[i];
        img[i].ToXYZ(p.xyz);
        p.filterWeightSum = 1;
        p.splatXYZ[0] = p.splatXYZ[1] = p.splatXYZ[2] = 0;
    }
}

void Film::AddSplat(const Point2f& p, Spectrum v) {
    if (!insideExclusive((Point2i)p, croppedPixelBounds)) {
        return;
    }
    if (v.y() > maxSampleLuminance) {
        v *= maxSampleLuminance / v.y();
    }
    Float xyz[3];
    v.ToXYZ(xyz);
    Pixel& pixel = GetPixel((Point2i)p);
    for (int i = 0; i < 3; ++i) {
        pixel.splatXYZ[i].add(xyz[i]);
    }
}

void Film::WriteImage(Float splatScale) {
    std::unique_ptr<Float[]> rgb(new Float[3 * croppedPixelBounds.area()]);
    int offset = 0;
    for (Point2i p : croppedPixelBounds) {
        // 将xyz转成rgb
        Pixel& pixel = GetPixel(p);
        XYZToRGB(pixel.xyz, &rgb[3 * offset]);

        // I(x,y) = (∑f(x-xi,y-yi)w(xi,yi)L(xi,yi)) / (∑f(x-xi,y-yi))
        // 再列一遍过滤表达式
        Float filterWeightNum = pixel.filterWeightSum;
        if (filterWeightNum != 0) {
            Float invWeight = (Float)1 / filterWeightNum;
            rgb[3 * offset] = std::max((Float)0, rgb[3 * offset] * invWeight);
            rgb[3 * offset + 1] = std::max((Float)0, rgb[3 * offset + 1] * invWeight);
            rgb[3 * offset + 2] = std::max((Float)0, rgb[3 * offset + 2] * invWeight);
        }

        // 这里splat是双向方法用的，暂时不理
        Float splatRGB[3];
        Float splatXYZ[3] = { pixel.splatXYZ[0],
                            pixel.splatXYZ[1],
                            pixel.splatXYZ[2] };
        XYZToRGB(splatXYZ, splatRGB);

        rgb[3 * offset] += splatScale * splatRGB[0];
        rgb[3 * offset + 1] += splatScale * splatRGB[1];
        rgb[3 * offset + 2] += splatScale * splatRGB[2];

        rgb[3 * offset] *= scale;
        rgb[3 * offset + 1] *= scale;
        rgb[3 * offset + 2] *= scale;
        ++offset;
    }
}

void Film::Clear() {
    for (Point2i p : croppedPixelBounds) {
        Pixel& pixel = GetPixel(p);
        for (int i = 0; i < 3; ++i) {
            pixel.xyz[i] = pixel.splatXYZ[i] = 0;
        }
        pixel.filterWeightSum = 0;
    }
}

RENDERING_END