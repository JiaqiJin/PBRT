#include "sampler.hpp"

#include "camera.hpp"

PALADIN_BEGIN

Sampler::Sampler(int64_t samplesPerPixel)
    : samplesPerPixel(samplesPerPixel) {

}

void Sampler::startPixel(const Point2i& p) {
    _currentPixel = p;
    _currentPixelSampleIndex = 0;
    _array1DOffset = _array2DOffset = 0;
}

CameraSample Sampler::getCameraSample(const Point2i& pRaster) {
    CameraSample ret;
    ret.pFilm = (Point2f)pRaster + get2D();
    ret.pLens = get2D();
    ret.time = get1D();
    return ret;
}

void Sampler::request1DArray(int n) {
    CHECK_EQ(n, roundCount(n));
    _samples1DArraySizes.push_back(n);
    _sampleArray1D.push_back(std::vector<Float>(n * samplesPerPixel));
}

void Sampler::request2DArray(int n) {
    CHECK_EQ(n, roundCount(n));
    _samples2DArraySizes.push_back(n);
    _sampleArray2D.push_back(std::vector<Point2f>(n * samplesPerPixel));
}

const Float* Sampler::get1DArray(int n) {
    if (_array1DOffset == _sampleArray1D.size()) {
        return nullptr;
    }
    CHECK_EQ(_samples1DArraySizes[_array1DOffset], n);
    CHECK_LT(_currentPixelSampleIndex, samplesPerPixel);
    return &_sampleArray1D[_array1DOffset++][_currentPixelSampleIndex * n];
}

const Point2f* Sampler::get2DArray(int n) {
    if (_array2DOffset == _sampleArray2D.size())
        return nullptr;
    CHECK_EQ(_samples2DArraySizes[_array2DOffset], n);
    CHECK_LT(_currentPixelSampleIndex, samplesPerPixel);
    return &_sampleArray2D[_array2DOffset++][_currentPixelSampleIndex * n];
}

bool Sampler::startNextSample() {
    _array1DOffset = _array2DOffset = 0;
    return ++_currentPixelSampleIndex < samplesPerPixel;
}

bool Sampler::setSampleNumber(int64_t sampleNum) {
    _array1DOffset = _array2DOffset = 0;
    _currentPixelSampleIndex = sampleNum;
    return _currentPixelSampleIndex < samplesPerPixel;
}

// PixelSampler
PixelSampler::PixelSampler(int64_t samplesPerPixel, int nSampledDimensions)
    :Sampler(samplesPerPixel)
{
    for (int i = 0; i < nSampledDimensions; ++i)
    {
        _samples1D.push_back(std::vector<Float>(samplesPerPixel));
        _samples2D.push_back(std::vector<Point2f>(samplesPerPixel));
    }
}

bool PixelSampler::startNextSample(){
    _curDimension1D = _curDimension2D = 0;
    return Sampler::startNextSample();
}

bool PixelSampler::setSampleNumber(int64_t sampleNum) {
    _curDimension1D = _curDimension2D = 0;
    return Sampler::setSampleNumber(sampleNum);
}

Float PixelSampler::get1D() {
    CHECK_LT(_currentPixelSampleIndex, samplesPerPixel);
    if (_curDimension1D < _samples1D.size()) {
        return _samples1D[_curDimension1D++][_currentPixelSampleIndex];
    }
    else {

    }
}

Point2f PixelSampler::get2D() {
    CHECK_LT(_currentPixelSampleIndex, samplesPerPixel);
    if (_curDimension2D < _samples2D.size()) {
        return _samples2D[_curDimension2D++][_currentPixelSampleIndex];
    }
    else {
        //return Point2f(_rng.uniformFloat(), _rng.uniformFloat());
    }
}

PALADIN_END