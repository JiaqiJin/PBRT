#pragma once

#include "Rendering.h"
#include "Spectrum.h"
#include "Filter.h"
#include "../Tool/Parallel.h"

#include <memory>
#include <vector>

RENDER_BEGIN

struct FilmTilePixel
{
	Spectrum contribSum = 0.f;
	Float filterWeightSum = 0.f;
};

class Film
{
public:
	typedef std::shared_ptr<Film> ptr;

	Film(const Vector2i& resolution, const Bounds2f& cropWindow,
		std::unique_ptr<Filter> filter, const std::string& filename, Float diagonal = 35.f,
		Float scale = 1.f, Float maxSampleLuminance = Infinity);

	Bounds2i getSampleBounds() const;

private:

};

class FilmTile final
{
public:

private:

};

RENDER_END