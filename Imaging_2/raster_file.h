#if !defined(RASTER_FILE_H)
#define RASTER_FILE_H

#include <string>

#include "raster_data.h"

namespace Imaging_2
{
	enum class FileMode { Read, Write };

	class RasterFileI
	{
	public:
		virtual bool Open(const std::wstring &path, FileMode mode = FileMode::Read) = 0;
		virtual bool Close(void) = 0;

	protected:
		bool isOpened = false;
		//FileMode fileMode = FileMode::Read;

		RasterFrame cachedData;
		// cache = raster data
		// cache info {ROI, cached}
	};

	class RawRasterFile : public RasterFileI
	{
		virtual bool Open(const std::wstring &path, FileMode mode = FileMode::Read);
		virtual bool Close(void);

	};
}

#endif
