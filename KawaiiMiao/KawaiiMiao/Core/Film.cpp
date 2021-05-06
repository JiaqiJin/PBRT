#include "Film.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../extern/stb_image_write.h"

RENDER_BEGIN

Film::Film(const Vector2i& resolution, const Bounds2f& cropWindow, std::unique_ptr<Filter> filter,
	const std::string& filename, Float diagonal, Float scale, Float maxSampleLuminance)
	: m_resolution(resolution), m_filter(std::move(filter)), m_diagonal(diagonal),
	m_filename(filename), m_scale(scale), m_maxSampleLuminance(maxSampleLuminance)
{
	//Compute film image bounds
	//Note: cropWindow range [0,1]x[0,1]
	m_croppedPixelBounds =
		Bounds2i(
			Vector2i(glm::ceil(m_resolution.x * cropWindow.m_pMin.x),
				glm::ceil(m_resolution.y * cropWindow.m_pMin.y)),
			Vector2i(glm::ceil(m_resolution.x * cropWindow.m_pMax.x),
				glm::ceil(m_resolution.y * cropWindow.m_pMax.y)));
	K_INFO("Created film with full resolution ", resolution.x, resolution.y, ". Crop window of ", cropWindow,
		" -> croppedPixelBounds ", m_croppedPixelBounds);

	m_pixels = std::unique_ptr<APixel[]>(new APixel[m_croppedPixelBounds.area()]);

	//Precompute filter weight table
	//Note: we assume that filtering function f(x,y)=f(|x|,|y|)
	//      hence only store values for the positive quadrant of filter offsets.
	int offset = 0;
	for (int y = 0; y < filterTableWidth; ++y)
	{
		for (int x = 0; x < filterTableWidth; ++x, ++offset)
		{
			Vector2f p;
			p.x = (x + 0.5f) * m_filter->m_radius.x / filterTableWidth;
			p.y = (y + 0.5f) * m_filter->m_radius.y / filterTableWidth;
			m_filterTable[offset] = m_filter->evaluate(p);
		}
	}
}

Bounds2i Film::getSampleBounds() const
{
	Bounds2f floatBounds(
		floor(Vector2f(m_croppedPixelBounds.m_pMin) + Vector2f(0.5f, 0.5f) - m_filter->m_radius),
		ceil(Vector2f(m_croppedPixelBounds.m_pMax) - Vector2f(0.5f, 0.5f) + m_filter->m_radius));
	return (Bounds2i)floatBounds;
}

std::unique_ptr<FilmTile> Film::getFilmTile(const Bounds2i& sampleBounds)
{
	// Bound image pixels that samples in _sampleBounds_ contribute to
	Vector2f halfPixel = Vector2f(0.5f, 0.5f);
	Bounds2f floatBounds = (Bounds2f)sampleBounds;
	Vector2i p0 = (Vector2i)ceil(floatBounds.m_pMin - halfPixel - m_filter->m_radius);
	Vector2i p1 = (Vector2i)floor(floatBounds.m_pMax - halfPixel + m_filter->m_radius) + Vector2i(1, 1);
	Bounds2i tilePixelBounds = intersect(Bounds2i(p0, p1), m_croppedPixelBounds);
	return std::unique_ptr<FilmTile>(new FilmTile(tilePixelBounds, m_filter->m_radius,
		m_filterTable, filterTableWidth, m_maxSampleLuminance));
}

void Film::mergeFilmTile(std::unique_ptr<FilmTile> tile)
{
	std::lock_guard<std::mutex> lock(m_mutex);
	for (Vector2i pixel : tile->getPixelBounds())
	{
		// Merge _pixel_ into _Film::pixels_
		const FilmTilePixel& tilePixel = tile->getPixel(pixel);
		APixel& mergePixel = getPixel(pixel);
		Float xyz[3];
		tilePixel.contribSum.toXYZ(xyz);
		for (int i = 0; i < 3; ++i)
		{
			mergePixel.m_xyz[i] += xyz[i];
		}
		mergePixel.m_filterWeightSum += tilePixel.filterWeightSum;
	}
}

void Film::writeImageToFile(Float splatScale)
{
	std::cout << "Converting image to RGB and computing final weighted pixel values";
	std::unique_ptr<Float[]> rgb(new Float[3 * m_croppedPixelBounds.area()]);
	std::unique_ptr<Byte[]>  dst(new Byte[3 * m_croppedPixelBounds.area()]);
	int offset = 0;
	for (Vector2i p : m_croppedPixelBounds)
	{
		// Convert pixel XYZ color to RGB
		APixel& pixel = getPixel(p);
		XYZToRGB(pixel.m_xyz, &rgb[3 * offset]);

		// Normalize pixel with weight sum
		Float filterWeightSum = pixel.m_filterWeightSum;
		if (filterWeightSum != 0)
		{
			Float invWt = (Float)1 / filterWeightSum;
			rgb[3 * offset + 0] = glm::max((Float)0, rgb[3 * offset + 0] * invWt);
			rgb[3 * offset + 1] = glm::max((Float)0, rgb[3 * offset + 1] * invWt);
			rgb[3 * offset + 2] = glm::max((Float)0, rgb[3 * offset + 2] * invWt);
		}

		// Add splat value at pixel
		Float splatRGB[3];
		Float splatXYZ[3] = { pixel.m_splatXYZ[0], pixel.m_splatXYZ[1],  pixel.m_splatXYZ[2] };
		XYZToRGB(splatXYZ, splatRGB);
		rgb[3 * offset + 0] += splatScale * splatRGB[0];
		rgb[3 * offset + 1] += splatScale * splatRGB[1];
		rgb[3 * offset + 2] += splatScale * splatRGB[2];

		// Scale pixel value by _scale_
		rgb[3 * offset + 0] *= m_scale;
		rgb[3 * offset + 1] *= m_scale;
		rgb[3 * offset + 2] *= m_scale;

#define TO_BYTE(v) (uint8_t) clamp(255.f * gammaCorrect(v) + 0.5f, 0.f, 255.f)
		dst[3 * offset + 0] = TO_BYTE(rgb[3 * offset + 0]);
		dst[3 * offset + 1] = TO_BYTE(rgb[3 * offset + 1]);
		dst[3 * offset + 2] = TO_BYTE(rgb[3 * offset + 2]);

		++offset;
	}

	std::cout << "Writing image " << m_filename << " with bounds " << m_croppedPixelBounds;
	auto extent = m_croppedPixelBounds.diagonal();
	stbi_write_png(m_filename.c_str(),
		extent.x,
		extent.y,
		3,
		static_cast<void*>(dst.get()),
		extent.x * 3);
}

void Film::setImage(const Spectrum* img) const
{
	int nPixels = m_croppedPixelBounds.area();
	for (int i = 0; i < nPixels; ++i)
	{
		APixel& p = m_pixels[i];
		img[i].toXYZ(p.m_xyz);
		p.m_filterWeightSum = 1;
		p.m_splatXYZ[0] = p.m_splatXYZ[1] = p.m_splatXYZ[2] = 0;
	}
}

void Film::addSplat(const Vector2f& p, Spectrum v)
{
	//Note:Rather than computing the final pixel value as a weighted
	//     average of contributing splats, splats are simply summed.

	if (v.hasNaNs())
	{
		std::cout << stringPrintf("Ignoring splatted spectrum with NaN values "
			"at (%f, %f)", p.x, p.y);
		return;
	}
	else if (v.y() < 0.)
	{
		std::cout << stringPrintf("Ignoring splatted spectrum with negative "
			"luminance %f at (%f, %f)", v.y(), p.x, p.y);
		return;
	}
	else if (glm::isinf(v.y()))
	{
		std::cout << stringPrintf("Ignoring splatted spectrum with infinite "
			"luminance at (%f, %f)", p.x, p.y);
		return;
	}

	Vector2i pi = Vector2i(floor(p));
	if (!insideExclusive(pi, m_croppedPixelBounds))
		return;

	if (v.y() > m_maxSampleLuminance)
	{
		v *= m_maxSampleLuminance / v.y();
	}

	Float xyz[3];
	v.toXYZ(xyz);

	APixel& pixel = getPixel(pi);
	for (int i = 0; i < 3; ++i)
	{
		pixel.m_splatXYZ[i].add(xyz[i]);
	}
}

void Film::clear()
{
	for (Vector2i p : m_croppedPixelBounds)
	{
		APixel& pixel = getPixel(p);
		for (int c = 0; c < 3; ++c)
		{
			pixel.m_splatXYZ[c] = pixel.m_xyz[c] = 0;
		}
		pixel.m_filterWeightSum = 0;
	}
}

RENDER_END