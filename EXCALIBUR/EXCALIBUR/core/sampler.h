#pragma once

#include "Header.h"
#include "../math/rng.h"

RENDERING_BEGIN

class Sampler {
public:
    virtual ~Sampler();
    Sampler(int64_t samplesPerPixel);
    virtual void StartPixel(const Point2i& p);
    virtual Float Get1D() = 0;
    virtual Point2f Get2D() = 0;
    CameraSample GetCameraSample(const Point2i& pRaster);
    void Request1DArray(int n);
    void Request2DArray(int n);
    virtual int RoundCount(int n) const {
        return n;
    }
    const Float* Get1DArray(int n);
    const Point2f* Get2DArray(int n);
    virtual bool StartNextSample();
    virtual std::unique_ptr<Sampler> Clone(int seed) = 0;
    virtual bool SetSampleNumber(int64_t sampleNum);

    const int64_t samplesPerPixel;

protected:
    // 当前处理像素点
    Point2i currentPixel;
    // 当前处理的像素样本索引
    int64_t currentPixelSampleIndex;
    // 用于储存一二维样本数量的列表
    std::vector<int> samples1DArraySizes, samples2DArraySizes;
    std::vector<std::vector<Float>> sampleArray1D;
    std::vector<std::vector<Point2f>> sampleArray2D;

private:
    size_t array1DOffset, array2DOffset;
};

class PixelSampler : public Sampler {
public:
    PixelSampler(int64_t samplesPerPixel, int nSampledDimensions);
    bool StartNextSample();
    bool SetSampleNumber(int64_t);
    Float Get1D();
    Point2f Get2D();

private:
    std::vector<std::vector<Float>> samples1D;
    std::vector<std::vector<Point2f>> samples2D;
    int current1DDimension = 0, current2DDimension = 0;
    RNG rng;
};

RENDERING_END