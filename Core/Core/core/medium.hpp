#ifndef medium_hpp
#define medium_hpp
#include "header.h"

KAWAII_BEGIN

// 两个介质的相交处，nullptr表示真空
struct MediumInterface {

    MediumInterface(const Medium* medium) : inside(medium), outside(medium) {

    }

    MediumInterface(const Medium* inside = nullptr, const Medium* outside = nullptr)
        :inside(inside),
        outside(outside) {

    }

    bool IsMediumTransition() const {
        return inside != outside;
    }

    const Medium* inside, * outside;
};

KAWAII_END

#endif /* medium_hpp */