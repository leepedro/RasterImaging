#if !defined(RASTER_FILE_H)
#define RASTER_FILE_H

#include <string>
#include <fstream>
#include <vector>

#include "raster_data.h"

namespace Imaging_2
{
	class FileI
	{
	public:
		virtual bool Open(const std::wstring &path, std::ios_base::openmode mode) = 0;
		virtual void Close(void) = 0;
	};

	class RasterFileI : public FileI
	{
	public:
		virtual bool Open(const std::wstring &path, std::ios_base::openmode mode) = 0;
		virtual void Close(void) = 0;

	protected:
		//bool isOpened = false;
		//bool isCached = false;
		//FileMode fileMode = FileMode::Read;

		RasterFrame cachedData;
		BlockRange cachedRange;
		// cache = raster data
		// cache info {ROI, cached}
	};

	class RawFile : public FileI
	{
	public:
		RawFile(void) = default;
		virtual ~RawFile(void) { this->Close(); }
		virtual bool Open(const std::wstring &path, std::ios_base::openmode mode);
		virtual void Close(void);

		template <typename T>
		void Write(const T *data, std::streamsize count);
		void Read(void);
		const std::vector<char> &cache = this->temp;

	protected:
		std::streamsize CheckLength(void);

		std::fstream fileStream;
		std::vector<char> temp;
	};

	// Write data from the current position of the file.
	template <typename T>
	void RawFile::Write(const T *data, std::streamsize count)
	{
		this->fileStream.write(reinterpret_cast<const char *>(data), sizeof(T) * count);
	}
}

#endif
