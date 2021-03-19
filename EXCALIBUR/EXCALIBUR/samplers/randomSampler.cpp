#include "randomSampler.h"
#include "../core/Sampling.h"

RENDERING_BEGIN

RandomSampler::RandomSampler(int ns, int seed) : Sampler(ns), rng(seed) {}

Float RandomSampler::Get1D() {
	CHECK_LT(currentPixelSampleIndex, samplesPerPixel);
	return rng.uniformFloat();
}

Point2f RandomSampler::Get2D() {
    CHECK_LT(currentPixelSampleIndex, samplesPerPixel);
    return { rng.uniformFloat(), rng.uniformFloat() };
}

std::unique_ptr<Sampler> RandomSampler::Clone(int seed) {
    RandomSampler* rs = new RandomSampler(*this);
    rs->rng.setSequence(seed);
    return std::unique_ptr<Sampler>(rs);
}

void RandomSampler::StartPixel(const Point2i& p) {
    for (size_t i = 0; i < sampleArray1D.size(); ++i)
        for (size_t j = 0; j < sampleArray1D[i].size(); ++j)
            sampleArray1D[i][j] = rng.uniformFloat();

    for (size_t i = 0; i < sampleArray2D.size(); ++i)
        for (size_t j = 0; j < sampleArray2D[i].size(); ++j)
            sampleArray2D[i][j] = { rng.uniformFloat(), rng.uniformFloat() };
    Sampler::StartPixel(p);
}

RENDERING_END