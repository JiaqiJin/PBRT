#include "RandomSampler.h"

RENDER_BEGIN

RandomSampler::RandomSampler(int ns, int seed)
	: Sampler(ns), m_rng(seed)
{

}

Float RandomSampler::get1D()
{
	CHECK_LT(m_currentPixelSampleIndex, samplesPerPixel);
	return m_rng.uniformFloat();
}

Vector2f RandomSampler::get2D()
{
	CHECK_LT(m_currentPixelSampleIndex, samplesPerPixel);
	return { m_rng.uniformFloat(), m_rng.uniformFloat() };
}

std::unique_ptr<Sampler> RandomSampler::clone(int seed)
{
	RandomSampler* rs = new RandomSampler(*this);
	rs->m_rng.setSequence(seed);
	return std::unique_ptr<Sampler>(rs);
}

void RandomSampler::startPixel(const Vector2i& p)
{
	for (size_t i = 0; i < m_sampleArray1D.size(); ++i)
		for (size_t j = 0; j < m_sampleArray1D[i].size(); ++j)
			m_sampleArray1D[i][j] = m_rng.uniformFloat();

	for (size_t i = 0; i < m_sampleArray2D.size(); ++i)
		for (size_t j = 0; j < m_sampleArray2D[i].size(); ++j)
			m_sampleArray2D[i][j] = { m_rng.uniformFloat(), m_rng.uniformFloat() };

	Sampler::startPixel(p);
}

RENDER_END