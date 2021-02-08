#ifndef mirror_hpp
#define mirror_hpp

#include "material.hpp"

PALADIN_BEGIN

class MirrorMaterial : public Material {

public:
    MirrorMaterial(const std::shared_ptr<Texture<Spectrum>>& r,
        const std::shared_ptr<Texture<Float>>& bump)
        :_Kr(r),
        _bumpMap(bump) {

    }

    virtual void computeScatteringFunctions(SurfaceInteraction* si, MemoryArena& arena,
        TransportMode mode,
        bool allowMultipleLobes) const;

private:
    std::shared_ptr<Texture<Spectrum>> _Kr;
    std::shared_ptr<Texture<Float>> _bumpMap;
};

PALADIN_END

#endif /* mirror_hpp */