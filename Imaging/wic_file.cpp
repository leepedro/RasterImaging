#include "wic_file.h"

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
		if (this->isOpened)
			this->Close();

		SafeRelease(this->factory);
	}

	bool WicFile::Open(const std::wstring &path, FileMode mode)
	{		
		if (mode == FileMode::Read)
		{	// Create a decoder for the whole file.
			HRESULT result(S_OK);
			result = this->factory->CreateDecoderFromFilename(path.c_str(), nullptr, GENERIC_READ,
				::WICDecodeMetadataCacheOnDemand, &this->decoder);
			if (result == 0x80070002)
				throw std::ios_base::failure("File not found!");
			else if (FAILED(result))
				throw std::runtime_error("Failed to create a decoder.");
		}
		else if (mode == FileMode::Write)
		{	// Create a file stream.
			SafeRelease(this->stream);
			if (FAILED(this->factory->CreateStream(&this->stream)))
				throw std::runtime_error("Failed to create a stream.");
			if (FAILED(this->stream->InitializeFromFilename(path.c_str(), GENERIC_WRITE)))
				throw std::runtime_error("Failed to initialize a stream.");

			GUID guid_container;
			if (!WicFile::GetContainerGUID(path, guid_container))
				throw std::runtime_error("Unknown container format.");

			// Create an encoder for a whole file.
			SafeRelease(this->encoder);
			if (FAILED(this->factory->CreateEncoder(guid_container, nullptr, &this->encoder)))
				throw std::runtime_error("Failed to create an encoder.");
		}
		else
			throw std::logic_error("Unknown file mode.");
		this->isOpened = true;

		return true;	// redundant response to maintain the class protocol.
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
		else if (ext.compare(L".png") == 0 || ext.compare(L".PNG") == 0)
			guid = GUID_ContainerFormatPng;
		else
			return false;
		return true;
	}

	bool WicFile::Read(RasterImage &imgDst, unsigned int frameNo)
	{
		::IWICBitmapFrameDecode *frame(nullptr);
		::IWICFormatConverter *format_converter(nullptr);
		try
		{
			// Get a frame.
			if (FAILED(decoder->GetFrame(frameNo, &frame)))
				throw std::runtime_error("Failed to get a frame.");

			// Create a format converter,
			// and convert the source image frame to 32bit BGRA.
			if (FAILED(this->factory->CreateFormatConverter(&format_converter)))
				throw std::runtime_error("Failed to create a format converter.");
			if (FAILED(format_converter->Initialize(frame, ::GUID_WICPixelFormat32bppPBGRA, ::WICBitmapDitherTypeNone,
				nullptr, 0.0, ::WICBitmapPaletteTypeCustom)))
				throw std::runtime_error("Failed to convert the source image frame.");

			// Copy image data to the destination.
			// Set the size with unsigned int instead of ::size_t because ::size_t (== unsigned long)
			// can be wider than unsigned int.
			unsigned int w, h;
			if (FAILED(format_converter->GetSize(&w, &h)))
				throw std::runtime_error("Failed to get the size of the source image frame.");
			unsigned int sz = w * h * 4;
			imgDst.Resize(4, w, h);
			if (FAILED(format_converter->CopyPixels(nullptr, w * 4, sz, imgDst.data.data())))
				throw std::runtime_error("Failed to copy pixels from the source image frame.");
		}
		catch (const std::exception &ex)
		{
			SafeRelease(format_converter);
			SafeRelease(frame);
			throw ex;	// Re-throw the exception.
		}

		SafeRelease(format_converter);
		SafeRelease(frame);

		return true;	// redundant response to maintain the class protocol.
	}

	bool WicFile::Write(const RasterImage &imgDst, unsigned int frameNo)
	{
		return false;
	}

	bool WicFile::Close(void)
	{
		SafeRelease(this->decoder);
		SafeRelease(this->encoder);
		SafeRelease(this->stream);
		this->isOpened = false;
		return true;
	}
}