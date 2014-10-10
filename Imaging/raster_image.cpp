#include "raster_image.h"

#include <iostream>

namespace Imaging
{
	//void RasterImage::Resize(DataType dataType, ::size_t depth, ::size_t width, ::size_t height)
	//{
	//	auto sz = depth * width * height;
	//	if (this->data.size() != sz)
	//		this->data.resize(sz);
	//	this->depth = depth;
	//	this->width = width;
	//	this->height = height;
	//	this->dataType = dataType;
	//}

	//template <typename T>
	//void RasterImage::Resize_(::size_t depth, ::size_t width, ::size_t height)
	//{

	//}

	void RasterImage::Resize(::size_t bpc, ::size_t d, ::size_t w, ::size_t h)
	{
		auto bytes_per_line = bpc * d * w;	// No zero padding.
		auto sz = bytes_per_line * h;

		// Resize only if necessary.
		if (this->data.size() != sz)
			this->data.resize(sz);
	}

	void RasterImage::Swap(std::vector<unsigned char> &src, ::size_t depth, ::size_t width, ::size_t height)
	{
		auto sz = depth * width * height;
		if (src.size() == sz)
			std::swap(src, this->data);
		this->depth = depth;
		this->width = width;
		this->height = height;
	}

	bool RasterImage::CheckBoundary(void) const
	{
		auto sz = this->depth * this->width * this->height;
		return this->data.size() == sz;
	}
}
