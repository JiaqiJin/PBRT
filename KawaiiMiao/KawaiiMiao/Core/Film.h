#pragma once

#include "Rendering.h"
#include "Spectrum.h"
#include "Filter.h"
#include "Rtti.h"
#include "../Tool/Parallel.h"
#include "../Math/KMathUtil.h"

#include <memory>
#include <vector>

RENDER_BEGIN

struct FilmTilePixel
{
	Spectrum contribSum = 0.f;
	Float filterWeightSum = 0.f;
};

class Film final : public AObject
{
public:
	typedef std::shared_ptr<Film> ptr;

	Film(const APropertyTreeNode& node);
	Film(const Vector2i& resolution, const Bounds2f& cropWindow,
		std::unique_ptr<Filter> filter, const std::string& filename, Float diagonal = 35.f,
		Float scale = 1.f, Float maxSampleLuminance = Infinity);

	Bounds2i getSampleBounds() const;
	const Vector2i getResolution() const { return m_resolution; }

	std::unique_ptr<FilmTile> getFilmTile(const Bounds2i& sampleBounds);
	void mergeFilmTile(std::unique_ptr<FilmTile> tile);

	void writeImageToFile(Float splatScale = 1);

	void setImage(const Spectrum* img) const;
	void addSplat(const Vector2f& p, Spectrum v);

	void clear();

private:

	//Note: XYZ is a display independent representation of color,
	//      and this is why we choose to use XYZ color herein.
	struct APixel
	{
		APixel()
		{
			m_xyz[0] = m_xyz[1] = m_xyz[2] = m_filterWeightSum = 0;
		}

		Float m_xyz[3];				//xyz color of the pixel
		Float m_filterWeightSum;	//the sum of filter weight values
		AtomicFloat m_splatXYZ[3]; //unweighted sum of samples splats
		Float m_pad;				//unused, ensure sizeof(APixel) -> 32 bytes
	};

	const Vector2i m_resolution; //(width, height)
	const std::string m_filename;
	std::unique_ptr<APixel[]> m_pixels;

	const Float m_diagonal;
	Bounds2i m_croppedPixelBounds;	//actual rendering window

	std::unique_ptr<Filter> m_filter;
	std::mutex m_mutex;

	//Note: precomputed filter weights table
	static constexpr int filterTableWidth = 16;
	Float m_filterTable[filterTableWidth * filterTableWidth];

	const Float m_scale;
	const Float m_maxSampleLuminance;

	APixel& getPixel(const Vector2i& p)
	{
		DCHECK(insideExclusive(p, m_croppedPixelBounds));
		int width = m_croppedPixelBounds.m_pMax.x - m_croppedPixelBounds.m_pMin.x;
		int index = (p.x - m_croppedPixelBounds.m_pMin.x) + (p.y - m_croppedPixelBounds.m_pMin.y) * width;
		return m_pixels[index];
	}

};

class FilmTile final
{
public:
	// FilmTile Public Methods
	FilmTile(const Bounds2i& pixelBounds, const Vector2f& filterRadius, const Float* filterTable,
		int filterTableSize, Float maxSampleLuminance)
		: m_pixelBounds(pixelBounds), m_filterRadius(filterRadius),
		m_invFilterRadius(1 / filterRadius.x, 1 / filterRadius.y),
		m_filterTable(filterTable), m_filterTableSize(filterTableSize),
		m_maxSampleLuminance(maxSampleLuminance)
	{
		m_pixels = std::vector<FilmTilePixel>(glm::max(0, pixelBounds.area()));
	}

	void addSample(const Vector2f& pFilm, Spectrum L, Float sampleWeight = 1.f)
	{
		if (L.y() > m_maxSampleLuminance)
			L *= m_maxSampleLuminance / L.y();

		// Compute sample's raster bounds
		Vector2f pFilmDiscrete = pFilm - Vector2f(0.5f, 0.5f);
		Vector2i p0 = (Vector2i)ceil(pFilmDiscrete - m_filterRadius);
		Vector2i p1 = (Vector2i)floor(pFilmDiscrete + m_filterRadius) + Vector2i(1, 1);
		p0 = max(p0, m_pixelBounds.m_pMin);
		p1 = min(p1, m_pixelBounds.m_pMax);

		// Loop over filter support and add sample to pixel arrays

		// Precompute $x$ and $y$ filter table offsets
		int* ifx = ALLOCA(int, p1.x - p0.x);
		for (int x = p0.x; x < p1.x; ++x)
		{
			Float fx = glm::abs((x - pFilmDiscrete.x) * m_invFilterRadius.x * m_filterTableSize);
			ifx[x - p0.x] = glm::min((int)glm::floor(fx), m_filterTableSize - 1);
		}

		int* ify = ALLOCA(int, p1.y - p0.y);
		for (int y = p0.y; y < p1.y; ++y)
		{
			Float fy = std::abs((y - pFilmDiscrete.y) * m_invFilterRadius.y * m_filterTableSize);
			ify[y - p0.y] = glm::min((int)std::floor(fy), m_filterTableSize - 1);
		}

		for (int y = p0.y; y < p1.y; ++y)
		{
			for (int x = p0.x; x < p1.x; ++x)
			{
				// Evaluate filter value at $(x,y)$ pixel
				int offset = ify[y - p0.y] * m_filterTableSize + ifx[x - p0.x];
				Float filterWeight = m_filterTable[offset];

				// Update pixel values with filtered sample contribution
				FilmTilePixel& pixel = getPixel(Vector2i(x, y));
				pixel.contribSum += L * sampleWeight * filterWeight;
				pixel.filterWeightSum += filterWeight;
			}
		}
	}

	FilmTilePixel& getPixel(const Vector2i& p)
	{
		DCHECK(insideExclusive(p, m_pixelBounds));
		int width = m_pixelBounds.m_pMax.x - m_pixelBounds.m_pMin.x;
		int index = (p.x - m_pixelBounds.m_pMin.x) + (p.y - m_pixelBounds.m_pMin.y) * width;
		return m_pixels[index];
	}

	const FilmTilePixel& getPixel(const Vector2i& p) const
	{
		DCHECK(insideExclusive(p, m_pixelBounds));
		int width = m_pixelBounds.m_pMax.x - m_pixelBounds.m_pMin.x;
		int index = (p.x - m_pixelBounds.m_pMin.x) + (p.y - m_pixelBounds.m_pMin.y) * width;
		return m_pixels[index];
	}

	Bounds2i getPixelBounds() const { return m_pixelBounds; }

private:
	const Bounds2i m_pixelBounds;
	const Vector2f m_filterRadius, m_invFilterRadius;
	const Float* m_filterTable;
	const int m_filterTableSize;
	std::vector<FilmTilePixel> m_pixels;
	const Float m_maxSampleLuminance;

	friend class Film;
};

RENDER_END