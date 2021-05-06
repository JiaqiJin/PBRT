#pragma once

#include "Rendering.h"
#include "../Math/KMathUtil.h"

RENDER_BEGIN

class Distribution1D
{
public:
	Distribution1D(const Float* f, int n) : func(f, f + n), cdf(n + 1)
	{
		// Compute integral of step function at $x_i$
		cdf[0] = 0;
		for (int i = 1; i < n + 1; ++i)
			cdf[i] = cdf[i - 1] + func[i - 1] / n;

		// Transform step function integral into CDF
		funcInt = cdf[n];
		if (funcInt == 0)
		{
			for (int i = 1; i < n + 1; ++i)
				cdf[i] = Float(i) / Float(n);
		}
		else
		{
			for (int i = 1; i < n + 1; ++i)
				cdf[i] /= funcInt;
		}
	}

	int count() const
	{
		return (int)func.size();
	}

	Float sampleContinuous(Float u, Float* pdf, int* off = nullptr) const
	{
		// Find surrounding CDF segments and _offset_
		int offset = findInterval((int)cdf.size(), [&](int index)
			{
				return cdf[index] <= u;
			});

		if (off)
			*off = offset;

		// Compute offset along CDF segment
		Float du = u - cdf[offset];
		if ((cdf[offset + 1] - cdf[offset]) > 0)
		{
			CHECK_GT(cdf[offset + 1], cdf[offset]);
			du /= (cdf[offset + 1] - cdf[offset]);
		}
		DCHECK(!glm::isnan(du));

		// Compute PDF for sampled offset
		if (pdf)
			*pdf = (funcInt > 0) ? func[offset] / funcInt : 0;

		// Return $x\in{}[0,1)$ corresponding to sample
		return (offset + du) / count();
	}

	int sampleDiscrete(Float u, Float* pdf = nullptr, Float* uRemapped = nullptr) const
	{
		// Find surrounding CDF segments and _offset_
		int offset = findInterval((int)cdf.size(), [&](int index)
			{
				return cdf[index] <= u;
			});

		if (pdf)
			*pdf = (funcInt > 0) ? func[offset] / (funcInt * count()) : 0;
		if (uRemapped)
			*uRemapped = (u - cdf[offset]) / (cdf[offset + 1] - cdf[offset]);
		if (uRemapped)
			DCHECK(*uRemapped >= 0.f && *uRemapped <= 1.f);
		return offset;
	}

	Float discretePDF(int index) const
	{
		DCHECK(index >= 0 && index < count());
		return func[index] / (funcInt * count());
	}

	std::vector<Float> func, cdf;
	Float funcInt;
};

RENDER_END