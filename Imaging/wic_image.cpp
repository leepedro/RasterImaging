#include "wic_image.h"

// Standard C++ header files.
#include <iostream>

// Windows header files.
#include <Shlwapi.h>


namespace Imaging
{
	WicFile::WicFile(void)
	{
		if (FAILED(::CoCreateInstance(::CLSID_WICImagingFactory, nullptr, CLSCTX_ALL, IID_PPV_ARGS(&this->factory))))
			throw std::runtime_error("Failed to create a WIC imaging factory.");
	}
	WicFile::~WicFile(void)
	{
		SafeRelease(this->decoder);
		SafeRelease(this->encoder);
		SafeRelease(this->stream);
		SafeRelease(this->factory);
	}

	bool WicFile::Open(const std::wstring &path, FileMode mode)
	{		
		if (mode == FileMode::Read)
		{	
			HRESULT result(S_OK);
			result = this->factory->CreateDecoderFromFilename(path.c_str(), nullptr, GENERIC_READ,
				::WICDecodeMetadataCacheOnDemand, &this->decoder);
			if (result == 0x80070002)
				throw std::runtime_error("File not found!");
			else if (FAILED(result))
				throw std::runtime_error("Failed to create a decoder.");
		}
		else if (mode == FileMode::Write)
		{
			SafeRelease(this->stream);
			if (FAILED(this->factory->CreateStream(&this->stream)))
				throw std::runtime_error("Failed to create a stream.");
			if (FAILED(this->stream->InitializeFromFilename(path.c_str(), GENERIC_WRITE)))
				throw std::runtime_error("Failed to initialize a stream.");

			GUID guid_container;
			if (!WicFile::GetContainerGUID(path, guid_container))
				throw std::runtime_error("Unknown container format.");

			SafeRelease(this->encoder);
			if (FAILED(this->factory->CreateEncoder(guid_container, nullptr, &this->encoder)))
				throw std::runtime_error("Failed to create an encoder.");
		}
		else
			throw std::logic_error("Unknown file mode.");

		this->isOpened = true;
		return true;
	}

	bool WicFile::GetContainerGUID(const std::wstring &path, GUID &guid)
	{
		std::wstring ext = ::PathFindExtensionW(path.c_str());
		if (ext.compare(L".bmp") == 0 || ext.compare(L".BMP") == 0)
			guid = GUID_ContainerFormatBmp;
		else if (ext.compare(L".jpg") == 0 || ext.compare(L".JPG") == 0 || ext.compare(L".jpeg") == 0 || ext.compare(L".JPEG") == 0)
			guid = GUID_ContainerFormatJpeg;
		else if (ext.compare(L".tif") == 0 || ext.compare(L".TIF") == 0 || ext.compare(L".tiff") == 0 || ext.compare(L".TIFF") == 0)
			guid = GUID_ContainerFormatTiff;
		else
			return false;
		return true;
	}

	bool WicFile::Read(RasterImage &imgDst, unsigned int frameNo)
	{
		return false;
	}

	bool WicFile::Write(const RasterImage &imgDst, unsigned int frameNo)
	{
		return false;
	}

	bool WicFile::Close(void)
	{
		this->isOpened = false;
		return false;
	}
}