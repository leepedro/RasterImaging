#include "raster_data.h"

#include <stdexcept>

namespace Imaging_2
{
	RasterFrame::RasterFrame(const RasterFrame &src) :
		data(src.data),
		bytesPerCh_(src.bytesPerCh), chPerPixel_(src.chPerPixel),
		width_(src.width), height_(src.height), bytesPerLine_(src.bytesPerLine) {}

	RasterFrame &RasterFrame::operator=(const RasterFrame &src)
	{
		if (this != &src)
		{
			this->data = src.data;

			this->bytesPerCh_ = src.bytesPerCh;
			this->chPerPixel_ = src.chPerPixel;
			this->width_ = src.width;
			this->height_ = src.height;
			this->bytesPerLine_ = src.bytesPerLine;
		}
		return *this;
	}

	RasterFrame::RasterFrame(RasterFrame &&src) :
		data(std::move(src.data)),
		bytesPerCh_(src.bytesPerCh), chPerPixel_(src.chPerPixel),
		width_(src.width), height_(src.height), bytesPerLine_(src.bytesPerLine) {}

	RasterFrame &RasterFrame::operator=(RasterFrame &&src)
	{
		if (this != &src)
			this->Swap(src);
		return *this;
	}

	void RasterFrame::Swap(RasterFrame &other)
	{
		// Q. Does following line actually copy data or just switch the pointer?
		this->data.swap(other.data);

		std::swap(this->bytesPerCh_, other.bytesPerCh_);
		std::swap(this->chPerPixel_, other.chPerPixel_);
		std::swap(this->width_, other.width_);
		std::swap(this->height_, other.height_);
		std::swap(this->bytesPerLine_, other.bytesPerLine_);
	}

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