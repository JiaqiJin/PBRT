#pragma once

#include "Header.h"
#include "spectrum.h"
#include "filter.h"
#include "../parallel/Parallel.h"

RENDERING_BEGIN

struct FilmTilePixel {
	Spectrum contribSum = 0.f;
	Float filterWeightSum = 0.f;
};

class Film {
public:
	Film(const Point2i& resolution, const AABB2i& cropWindow,
		std::unique_ptr<Filter> filter, Float diagonal,
		const std::string& filename, Float scale,
		Float maxSampleLuminance = Infinity);

	AABB2i GetSampleBounds() const;
	AABB2f GetPhysicalExtent() const;
	std::unique_ptr<FilmTile> GetFilmTile(const AABB2i& sampleBounds);
	void MergeFilmTile(std::unique_ptr<FilmTile> tile);
	void SetImage(const Spectrum* img) const;
	void AddSplat(const Point2f& p, Spectrum v);
	void WriteImage(Float splatScale = 1);
	void Clear();

	const Point2f fullResolution;
	const Float diagonal;
	std::unique_ptr<Filter> filter;
	const std::string filename;
	AABB2i croppedPixelBounds;
	
private:
	struct Pixel {
		Float xyz[3] = { 0,0,0 };
		// 所有样本的filter权重之和
		Float filterWeightSum = 0;
		// 保存（未加权）样本splats总和
		AtomicFloat splatXYZ[3];
		Float pad;
	};
	std::unique_ptr<Pixel[]> pixels;
	static constexpr int filterTableWidth = 16;
	Float filterTable[filterTableWidth * filterTableWidth];
	std::mutex mutex;
	Float scale;
	Float maxSampleLuminance;

	Pixel& GetPixel(const Point2i& p) {
		DCHECK(insideExclusive(p, croppedPixelBounds));
		int width = croppedPixelBounds.pMax.x - croppedPixelBounds.pMin.x;
		int offset = (p.x - croppedPixelBounds.pMin.x) +
			(p.y - croppedPixelBounds.pMin.y) * width;
		return pixels[offset];
	}
};

// 可以理解为胶片类，相当于相机中接收光子 传感器
class FilmTile {
public:
	FilmTile(const AABB2i& pixelBounds, const Vector2f& filterRadius,
		const Float* filterTable, int filterTableSize,
		Float maxSampleLuminance)
		: pixelBounds(pixelBounds),
		filterRadius(filterRadius),
		invFilterRadius(1 / filterRadius.x, 1 / filterRadius.y),
		filterTable(filterTable),
		filterTableSize(filterTableSize),
		maxSampleLuminance(maxSampleLuminance) {
		pixels = std::vector<FilmTilePixel>(std::max(0, pixelBounds.area()));
	}

	void AddSample(const Point2f& pFilm, const Spectrum& L,
		Float sampleWeight = 1.) {

	}

	FilmTilePixel& GetPixel(const Point2i& p) {
		int width = pixelBounds.pMax.x - pixelBounds.pMin.x;
		int offset = (p.x - pixelBounds.pMin.x) +
			(p.y - pixelBounds.pMin.y) * width;
		return pixels[offset];
	}

	const FilmTilePixel& GetPixel(const Point2i& p) const {
		int width = pixelBounds.pMax.x - pixelBounds.pMin.x;
		int offset = (p.x - pixelBounds.pMin.x) +
			(p.y - pixelBounds.pMin.y) * width;
		return pixels[offset];
	}

	AABB2i GetPixelBounds() const { return pixelBounds; }

private:
	const AABB2i pixelBounds;
	const Vector2f filterRadius, invFilterRadius;
	const Float* filterTable;
	const int filterTableSize;
	std::vector<FilmTilePixel> pixels;
	const Float maxSampleLuminance;
	friend class Film;
};

RENDERING_END