#include "Sampler.h"

#include "Camera.h"

RENDER_BEGIN

Sampler::~Sampler() {}

Sampler::Sampler(int64_t samplesPerPixel) 
	: samplesPerPixel(samplesPerPixel) 
{

}

CameraSample Sampler::getCameraSample(const Vector2i& pRaster)
{
	CameraSample cs;
	cs.pFilm = (Vector2f)pRaster + get2D();
	return cs;
}

void Sampler::startPixel(const Vector2i& p)
{
	m_currentPixel = p;
	m_currentPixelSampleIndex = 0;
	// Reset array offsets for next pixel sample
	m_array1DOffset = m_array2DOffset = 0;
}

bool Sampler::startNextSample()
{
	// Reset array offsets for next pixel sample
	m_array1DOffset = m_array2DOffset = 0;
	return ++m_currentPixelSampleIndex < samplesPerPixel;
}

bool Sampler::setSampleNumber(int64_t sampleNum)
{
	// Reset array offsets for next pixel sample
	m_array1DOffset = m_array2DOffset = 0;
	m_currentPixelSampleIndex = sampleNum;
	return m_currentPixelSampleIndex < samplesPerPixel;
}

void Sampler::request1DArray(int n)
{
	CHECK_EQ(roundCount(n), n);
	m_samples1DArraySizes.push_back(n);
	m_sampleArray1D.push_back(std::vector<Float>(n * samplesPerPixel));
}

void Sampler::request2DArray(int n)
{
	CHECK_EQ(roundCount(n), n);
	m_samples2DArraySizes.push_back(n);
	m_sampleArray2D.push_back(std::vector<Vector2f>(n * samplesPerPixel));
}

const Float* Sampler::get1DArray(int n)
{
	if (m_array1DOffset == m_sampleArray1D.size())
		return nullptr;
	CHECK_EQ(m_samples1DArraySizes[m_array1DOffset], n);
	CHECK_LT(m_currentPixelSampleIndex, samplesPerPixel);
	return &m_sampleArray1D[m_array1DOffset++][m_currentPixelSampleIndex * n];
}

const Vector2f* Sampler::get2DArray(int n)
{
	if (m_array2DOffset == m_sampleArray2D.size())
		return nullptr;
	CHECK_EQ(m_samples2DArraySizes[m_array2DOffset], n);
	CHECK_LT(m_currentPixelSampleIndex, samplesPerPixel);
	return &m_sampleArray2D[m_array2DOffset++][m_currentPixelSampleIndex * n];
}


RENDER_END