#include "imagemap.h"

RENDERING_BEGIN

template <typename Tmemory, typename Treturn>
std::map<TexInfo, std::unique_ptr<MIPMap<Tmemory>>>
ImageTexture<Tmemory, Treturn>::_imageCache;

std::shared_ptr<ImageTexture<RGBSpectrum, Spectrum>> createImageMap(const std::string& filename, bool gamma, bool doTri,
    Float maxAniso, ImageWrap wm, Float scale,
    bool doFilter,
    std::unique_ptr<TextureMapping2D> mapping) {
    return std::make_shared<ImageTexture<RGBSpectrum, Spectrum>>(move(mapping),
        filename,
        doTri, maxAniso,
        wm, scale, gamma, doFilter);
}

std::shared_ptr<ImageTexture<Float, Float>> createFloatMap(const std::string& filename, bool gamma, bool doTri,
    Float maxAniso, ImageWrap wm, Float scale,
    bool doFilter,
    std::unique_ptr<TextureMapping2D> mapping) {
    return std::make_shared<ImageTexture<Float, Float>>(std::move(mapping),
        filename,
        doTri, maxAniso,
        wm, scale, gamma, doFilter);
}


RENDERING_END