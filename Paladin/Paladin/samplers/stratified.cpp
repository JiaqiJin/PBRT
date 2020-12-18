﻿#include "stratified.hpp"
#include "sampling.hpp"

PALADIN_BEGIN

void StratifiedSampler::startPixel(const Point2i& p) {
    // 为每个像素生成一系列单独的样本
    size_t count = _xPixelSamples * _xPixelSamples;
    for (size_t i = 0; i < _samples1D.size(); ++i) {
        stratifiedSample1D(&_samples1D[i][0], count, _rng, _jitterSamples);
        shuffle(&_samples1D[i][0], count, 1, _rng);
    }
    for (size_t i = 0; i < _samples2D.size(); ++i) {
        stratifiedSample2D(&_samples2D[i][0], _xPixelSamples, _yPixelSamples, _rng, _jitterSamples);
        shuffle(&_samples2D[i][0], count, 1, _rng);
    }

    // 为每个像素生成一组样本
    for (size_t i = 0; i < _sampleArray1D.size(); ++i) {
        for (size_t j = 0; samplesPerPixel; ++j) {
            size_t count = _samples1DArraySizes[i];
            stratifiedSample1D(&_sampleArray1D[i][j * count], count, _rng, _jitterSamples);
            shuffle(&_sampleArray1D[i][j * count], count, 1, _rng);
        }
    }
    for (size_t i = 0; i < _sampleArray2D.size(); ++i) {
        for (size_t j = 0; samplesPerPixel; ++j) {
            size_t count = _samples2DArraySizes[i];
            latinHypercube(&_sampleArray2D[i][j * count].x, count, 2, _rng);
        }
    }
}

std::unique_ptr<Sampler> StratifiedSampler::clone(int seed) {
    StratifiedSampler* ret = new StratifiedSampler(*this);
    ret->_rng.setSequence(seed);
    return std::unique_ptr<Sampler>(ret);
}

PALADIN_END