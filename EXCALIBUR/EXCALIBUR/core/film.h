#pragma once

#include "Header.h"
#include "spectrum.h"
#include "filter.h"
#include "../parallel/Parallel.h"

RENDERING_BEGIN

class Film {
public:
	Film(const Point2i& resolution, const AABB2i& cropWindow,
		std::unique_ptr<Filter> filter, Float diagonal,
		const std::string& filename, Float scale,
		Float maxSampleLuminance = Infinity);

	AABB2i GetSampleBounds() const;
	AABB2f GetPhysicalExtent() const;
	//std::unique_ptr<FilmTile> GetFilmTile(const AABB2i& sampleBounds);
	//void MergeFilmTile(std::unique_ptr<FilmTile> tile);
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

};

RENDERING_END