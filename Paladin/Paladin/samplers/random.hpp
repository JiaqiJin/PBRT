#ifndef random_hpp
#define random_hpp

#include "header.h"
#include "sampler.hpp"

PALADIN_BEGIN

class RandomSampler : public Sampler {
public:
    RandomSampler(int ns, int seed = 0)
        : Sampler(ns),
        _rng(seed) {

    }

    virtual void startPixel(const Point2i&);

    virtual Float get1D();

    virtual Point2f get2D();

    virtual std::unique_ptr<Sampler> clone(int seed);

private:
    RNG _rng;
};

PALADIN_END

#endif /* random_hpp */
