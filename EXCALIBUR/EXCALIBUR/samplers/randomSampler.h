#pragma once

#include "../core/sampler.h"
#include "../math/rng.h"

RENDERING_BEGIN

class RandomSampler : public Sampler {
public:
	RandomSampler(int ns, int seed = 0);
	void StartPixel(const Point2i&);
	Float Get1D();
	Point2f Get2D();
	std::unique_ptr<Sampler> Clone(int seed);
private:
	RNG rng;
};

RENDERING_END