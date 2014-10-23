#if !defined(RASTER_DATA_H)
#define RASTER_DATA_H

#include <valarray>

namespace Imaging
{
	typedef ::size_t PosType;

	struct DataBlock
	{
	public:
		// Examples:
		// DataBlock roi1 = { 0, 1, 2, 3 };
		// DataBlock roi2{ 10, 11, 12, 13 };
		// The values are assigned in the order of declaration.
		PosType rowFirst, rowLast, columnFirst, columnLast;
		PosType GetWidth(void) { return this->columnLast - this->columnFirst + 1; }
		PosType GetHeight(void) { return this->rowLast - this->rowFirst + 1; }
	};

	class RasterFrame
	{
	public:
		void Resize(::size_t bytes, PosType d, PosType w, PosType h);
		unsigned char *GetPointer(PosType row, PosType col);
		const unsigned char *GetConstPointer(PosType row, PosType col) const;

		const ::size_t &bytesPerCh = this->bytesPerCh_;
		const PosType &depth = this->depth_;
		const PosType &width = this->width_;
		const ::size_t &bytesPerLine = this->bytesPerLine_;
		const PosType &height = this->height_;

	protected:
		std::valarray<unsigned char> data;
		::size_t bytesPerCh_ = 1;	// bytes / channel, (default) = 1
		PosType depth_ = 1;			// channels / pixel
		PosType width_ = 0;			// pixels / line
		PosType height_ = 0;		// lines / frame
		::size_t bytesPerLine_ = 0;			// bytes / line >= bytesPerCh x depth x width
	};
}

#endif