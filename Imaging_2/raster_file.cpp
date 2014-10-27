#include "raster_file.h"

namespace Imaging_2
{
	bool RawRasterFile::Open(const std::wstring &path, std::ios_base::openmode mode)
	{
		this->Close();
		this->fileStream.open(path, mode);
		return this->fileStream.is_open() && this->fileStream.good();
	}

	void RawRasterFile::Close(void)
	{
		if (this->fileStream.is_open())
			this->fileStream.close();
	}

	void RawRasterFile::Read(void)
	{
		// Get the length and allocate the memory.
		auto length = this->CheckLength();
		if (temp.size() != length)
			temp.resize(length);

		// Go back to the beginning of the file and read as long as the length.
		this->fileStream.seekg(0, std::ios_base::beg);
		this->fileStream.read(temp.data(), length);
	}

	// Compute the length of the file (as bytes?)
	// by the difference between the last file position and the first file position.
	std::streamsize RawRasterFile::CheckLength(void)
	{
		this->fileStream.seekg(0, std::ios_base::beg);
		auto begin = this->fileStream.tellg();
		this->fileStream.seekg(0, std::ios_base::end);
		return this->fileStream.tellg() - begin;
	}

}