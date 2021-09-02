#pragma once

#include "../Core/Sampler.h"
#include "../Math/Rng.h"

RENDER_BEGIN

class RandomSampler final : public Sampler
{
public:
	typedef std::shared_ptr<RandomSampler> ptr;

	RandomSampler(const APropertyTreeNode& node);
	RandomSampler(int ns, int seed = 0);

	virtual void startPixel(const Vector2i&) override;

	virtual Float get1D() override;
	virtual Vector2f get2D() override;

	virtual std::unique_ptr<Sampler> clone(int seed) override;

	virtual std::string toString() const override { return "RandomSampler[]"; }

private:
	Rng m_rng; //Random number generator
};
RENDER_END