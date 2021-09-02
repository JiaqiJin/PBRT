#pragma once

#include "Rendering.h"
#include "../Math/KMathUtil.h"
#include "Rtti.h"

#include <vector>

RENDER_BEGIN

class Sampler : public AObject
{
public:
	typedef std::shared_ptr<Sampler> ptr;

	virtual ~Sampler();
	Sampler(const APropertyList& props);
	Sampler(int64_t samplesPerPixel);

	virtual void startPixel(const Vector2i& p);
	virtual Float get1D() = 0;
	virtual Vector2f get2D() = 0;
	CameraSample getCameraSample(const Vector2i& pRaster);

	void request1DArray(int n);
	void request2DArray(int n);

	virtual int roundCount(int n) const { return n; }

	const Float* get1DArray(int n);
	const Vector2f* get2DArray(int n);

	virtual bool startNextSample();

	virtual std::unique_ptr<Sampler> clone(int seed) = 0;
	virtual bool setSampleNumber(int64_t sampleNum);

	int64_t currentSampleNumber() const { return m_currentPixelSampleIndex; }

	int64_t getSamplingNumber() const { return samplesPerPixel; }

	const int64_t samplesPerPixel; //Number of sampling per pixel

	virtual ClassType getClassType() const override { return ClassType::RSampler; }

protected:
	Vector2f m_currentPixel;
	int64_t m_currentPixelSampleIndex;
	std::vector<int> m_samples1DArraySizes, m_samples2DArraySizes;
	std::vector<std::vector<Float>> m_sampleArray1D;
	std::vector<std::vector<Vector2f>> m_sampleArray2D;

private:
	size_t m_array1DOffset, m_array2DOffset;
};


RENDER_END