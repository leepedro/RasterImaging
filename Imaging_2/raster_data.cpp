#include "raster_data.h"

#include <stdexcept>

namespace Imaging_2
{
	void RasterFrame::Resize(::size_t bytes, PosType ch, PosType w, PosType h)
	{		
		try
		{
			// Allocate memory.
			auto bytes_total = bytes * ch * w * h;
			this->data.resize(bytes_total);

			// Update data dimension.
			this->bytesPerCh_ = bytes;
			this->chPerPixel_ = ch;
			this->width_ = w;
			this->height_ = h;

			// Number of bytes / line including padding bytes.
			this->bytesPerLine_ = this->bytesPerCh * this->chPerPixel * this->width;
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
		auto offset = row * this->bytesPerLine + col * this->bytesPerCh * this->chPerPixel;
		return &this->data[offset];
	}

	const unsigned char *RasterFrame::GetConstPointer(PosType row, PosType col) const
	{
		auto offset = row * this->bytesPerLine + col * this->bytesPerCh * this->chPerPixel;
		return &this->data[offset];
	}
}