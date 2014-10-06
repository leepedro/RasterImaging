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

		std::vector<char> data;
		::size_t depth = 0;
		::size_t width = 0;
		::size_t height = 0;

	protected:
	private:
	};

	class RasterFileI
	{
	public:
		//virtual bool load(const std::wstring &pathSrc) = 0;
		//virtual bool save(const std::wstring &pathDst) const = 0;
		enum class FileMode { Read, Write };
		virtual bool Open(const std::wstring &path, FileMode mode = FileMode::Read) = 0;
		virtual bool Read(RasterImage &imgDst, unsigned int frameNo = 0) = 0;
		virtual bool Write(const RasterImage &imgDst, unsigned int frameNo = 0) = 0;
		virtual bool Close(void) = 0;

	protected:
		bool isOpened = false;

	private:
	};

}

#endif
