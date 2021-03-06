#include "raster_data.h"

#include <stdexcept>

namespace Imaging_2
{
	RasterFrame::RasterFrame(const RasterFrame &src) :
		data(src.data),
		bytesPerCh_(src.bytesPerCh), chPerPixel_(src.chPerPixel),
		width_(src.width), height_(src.height), bytesPerLine_(src.bytesPerLine) {}

	RasterFrame::RasterFrame(RasterFrame &&src) : RasterFrame()
	{
		this->Swap(src);
	}

	// Use copy and swap idiom for all of copy assignment and move assignment.
	// Argument is given as value.
	RasterFrame &RasterFrame::operator=(RasterFrame src)
	{
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
			// Allocate memory. Avoid re-allocation if possible.
			auto bytes_total = bytes * ch * w * h;
			if (this->data.size() != bytes_total)
				this->data.resize(bytes_total);

			this->ResetDimension(bytes, ch, w, h);
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

	// Resets dimension only if the given dimension matches with the size of existing memory block.
	bool RasterFrame::ResetDimension(::size_t bytes, PosType ch, PosType w, PosType h)
	{
		auto bytes_total = bytes * ch * w * h;
		if (this->data.size() == bytes_total)
		{
			this->SetDimension(bytes, ch, w, h);
			return true;
		}
		else
			return false;
	}

	void RasterFrame::SetDimension(::size_t bytes, PosType ch, PosType w, PosType h)
	{
		// Update data dimension.
		this->bytesPerCh_ = bytes;
		this->chPerPixel_ = ch;
		this->width_ = w;
		this->height_ = h;

		// Number of bytes / line including padding bytes.
		this->bytesPerLine_ = this->bytesPerCh * this->chPerPixel * this->width;
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