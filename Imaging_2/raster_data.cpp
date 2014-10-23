#include "raster_data.h"

#include <stdexcept>

namespace Imaging
{
	void RasterFrame::Resize(::size_t bytes, PosType d, PosType w, PosType h)
	{		
		try
		{
			auto bytes_total = bytes * d * w * h;
			this->data.resize(bytes_total);
			this->bytesPerCh_ = bytes;
			this->depth_ = d;
			this->width_ = w;
			this->height_ = h;

			this->bytesPerLine_ = this->bytesPerCh_ * this->depth_ * this->width_;
		}
		catch (const std::bad_alloc &ex)
		{
			throw ex;
		}
		catch (...)
		{
			throw std::runtime_error("Failed to resize.");
		}		
	}

	unsigned char *RasterFrame::GetPointer(PosType row, PosType col)
	{
		auto offset = row * this->bytesPerLine + col * this->bytesPerCh * this->depth;
		return &this->data[offset];
	}

	const unsigned char *RasterFrame::GetConstPointer(PosType row, PosType col) const
	{
		auto offset = row * this->bytesPerLine + col * this->bytesPerCh * this->depth;
		return &this->data[offset];
	}
}