#pragma once

#include "Rendering.h"
#include "../Math/KMathUtil.h"

RENDER_BEGIN

// Media Declarations
class PhaseFunction 
{
public:
    // PhaseFunction Interface
    virtual ~PhaseFunction();
    virtual Float p(const Vector3f& wo, const Vector3f& wi) const = 0;
    virtual Float Sample_p(const Vector3f& wo, Vector3f* wi, const Vector2f& u) const = 0;
    virtual std::string ToString() const = 0;
};

inline std::ostream& operator<<(std::ostream& os, const PhaseFunction& p) {
    os << p.ToString();
    return os;
}

class Medium
{
public:

private:

};

struct MediumInterface
{
    MediumInterface() : inside(nullptr), outside(nullptr) {}
    // MediumInterface Public Methods
    MediumInterface(const Medium* medium) : inside(medium), outside(medium) {}
    MediumInterface(const Medium* inside, const Medium* outside)
        : inside(inside), outside(outside) {}
    bool IsMediumTransition() const { return inside != outside; }
    const Medium* inside, * outside;
};

RENDER_END