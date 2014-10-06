#if !defined(RASTER_IMAGE_H)
#define RASTER_IMAGE_H

// Standard C++ header files.
#include <string>
#include <vector>

namespace Imaging
{
	class RasterImage
	{
	public:
		void Resize(::size_t depth, ::size_t width, ::size_t height);
		void Swap(std::vector<unsigned char> &src, ::size_t depth, ::size_t width, ::size_t height);

		std::vector<unsigned char> data;
		::size_t depth = 0;
		::size_t width = 0;
		::size_t height = 0;

	protected:
	private:
		bool CheckBoundary(void) const;
	};

	enum class FileMode { Read, Write };

	// Interface for general file IO
	// Each function may return the status of operation as a boolean value or
	// may throw an exception.
	// Detailed description should be parsed from the thrown exception object.
	class FileI
	{
	public:
		virtual bool Open(const std::wstring &path, FileMode mode = FileMode::Read) = 0;
		virtual bool Close(void) = 0;

	protected:
		bool isOpened = false;

	private:
	};

	// Interface for raster image files consisting of only image frames
	// without separate meta data sections.
	class RasterFileI : public FileI
	{
	public:
		virtual bool Open(const std::wstring &path, FileMode mode = FileMode::Read) = 0;
		virtual bool Read(RasterImage &imgDst, unsigned int frameNo = 0) = 0;
		virtual bool Write(const RasterImage &imgDst, unsigned int frameNo = 0) = 0;
		virtual bool Close(void) = 0;
	};


	// Future implementations.
	class Hdf5File : public FileI
	{
	public:
		bool Open(const std::wstring &path, FileMode mode = FileMode::Read) override;
		bool Close(void) override;
	};

	class NitfFile : FileI
	{
		bool Open(const std::wstring &path, FileMode mode = FileMode::Read) override;
		bool Close(void) override;
	};
}

#endif
