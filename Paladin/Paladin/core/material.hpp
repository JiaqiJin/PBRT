#ifndef material_hpp
#define material_hpp

#include "header.h"
#include "interaction.hpp"

PALADIN_BEGIN

enum class TransportMode { Radiance, Importance };

class Material {
public:
    virtual void computeScatteringFunctions(SurfaceInteraction* si,
        MemoryArena& arena,
        TransportMode mode,
        bool allowMultipleLobes) const = 0;
    virtual ~Material();
    static void bump(const std::shared_ptr<Texture<Float>>& d,
        SurfaceInteraction* si);
};

PALADIN_END
#endif /* material_hpp */
