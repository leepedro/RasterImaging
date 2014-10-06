#include "raster_image.h"

#include <iostream>

namespace Imaging
{
	void RasterImage::Resize(::size_t d, ::size_t width, ::size_t height)
	{
		auto sz = depth * width * height;
		if (this->data.size() != sz)
			this->data.resize(sz);
	}
}
