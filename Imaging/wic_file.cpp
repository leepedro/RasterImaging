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
		{	// Create and initialize a file stream.
			SafeRelease(this->stream);
			if (FAILED(this->factory->CreateStream(&this->stream)))
				throw std::runtime_error("Failed to create a stream.");
			if (FAILED(this->stream->InitializeFromFilename(path.c_str(), GENERIC_WRITE)))
				throw std::runtime_error("Failed to initialize a stream.");

			GUID guid_container;
			if (!WicFile::GetContainerGUID(path, guid_container))
				throw std::runtime_error("Unknown container format.");

			// Create and initialize an encoder for a whole file.
			SafeRelease(this->encoder);
			if (FAILED(this->factory->CreateEncoder(guid_container, nullptr, &this->encoder)))
				throw std::runtime_error("Failed to create an encoder.");
			if (FAILED(this->encoder->Initialize(this->stream, WICBitmapEncoderNoCache)))
				throw std::runtime_error("Failed to initialize an encoder.");
		}
		else
			throw std::logic_error("Unknown file mode.");
		this->fileMode = mode;
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
			// and convert the source image frame to 32bit BGRA regardless of original format.
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
			imgDst.Resize(1, 4, w, h);
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

	bool WicFile::Write(const RasterImage &imgSrc, unsigned int frameNo)
	{
		WICPixelFormatGUID pixel_fmt;
		IWICBitmapFrameEncode *frame(nullptr);
		try
		{
			if (FAILED(this->encoder->CreateNewFrame(&frame, nullptr)))
				throw std::runtime_error("Failed to create a frame.");
			if (FAILED(frame->Initialize(nullptr)))
				throw std::runtime_error("Failed to initialize a frame.");
			auto width = static_cast<unsigned int>(imgSrc.width);
			auto height = static_cast<unsigned int>(imgSrc.height);
			switch (imgSrc.depth)
			{
			case 1:
				pixel_fmt = GUID_WICPixelFormat8bppGray;
				if (FAILED(frame->SetPixelFormat(&pixel_fmt)))
					throw std::runtime_error("Failed to set pixel format.");
				if (::IsEqualGUID(pixel_fmt, GUID_WICPixelFormat8bppGray) != TRUE)
					//{				
					//	this->factory->CreateBitmap(imgSrc.width, imgSrc.height, GUID_WICPixelFormat8bppGray, WICBitmapCacheOnDemand, &bitmap);

					//	// copy pixels

					//	frame->SetSize(imgSrc.width, imgSrc.height);
					//	frame->WriteSource(bitmap, nullptr);
					//}
					//else if (::IsEqualGUID(pixel_fmt, GUID_WICPixelFormat8bppIndexed) == TRUE)
					//{
					//	IWICPalette *palette(nullptr);
					//	this->factory->CreatePalette(&palette);
					//	this->factory->CreateBitmap(imgSrc.width, imgSrc.height, GUID_WICPixelFormat8bppIndexed, WICBitmapCacheOnDemand, &bitmap);
					//	palette->InitializePredefined(WICBitmapPaletteTypeFixedGray256, FALSE);

					//	// copy pixels to bitmap
					//	WICRect rc = { 0, 0, imgSrc.width, imgSrc.height };
					//	IWICBitmapLock *bitmap_lock(nullptr);
					//	bitmap->Lock(&rc, WICBitmapLockWrite, &bitmap_lock);
					//	unsigned int stride(0);
					//	bitmap_lock->GetStride(&stride);
					//	unsigned int sz_buffer(0);
					//	unsigned char *dst(nullptr);
					//	bitmap_lock->GetDataPointer(&sz_buffer, &dst);

					//	//
					//	bitmap->SetPalette(palette);

					//}
					//else
					throw std::runtime_error("Unsupported pixel format");
				break;
			case 3:
				pixel_fmt = GUID_WICPixelFormat24bppBGR;
				if (FAILED(frame->SetPixelFormat(&pixel_fmt)))
					throw std::runtime_error("Failed to set pixel format.");
				if (::IsEqualGUID(pixel_fmt, GUID_WICPixelFormat24bppBGR) != TRUE)
					throw std::runtime_error("Unsupported pixel format");
				break;
			case 4:
				pixel_fmt = GUID_WICPixelFormat32bppBGRA;
				if (FAILED(frame->SetPixelFormat(&pixel_fmt)))
					throw std::runtime_error("Failed to set pixel format.");
				if (::IsEqualGUID(pixel_fmt, GUID_WICPixelFormat32bppBGRA) != TRUE)
					throw std::runtime_error("Unsupported pixel format");
				break;
			default:
				throw std::logic_error("Unsupported number of channels per pixel");
			}

			if (FAILED(frame->SetSize(width, height)))
				throw std::runtime_error("Failed to set the size of a frame.");
			unsigned int stride = width * static_cast<unsigned int>(imgSrc.depth);
			unsigned int sz_buffer = height * stride;
			if (FAILED(frame->WritePixels(height, stride, sz_buffer, const_cast<unsigned char *>(imgSrc.data.data()))))
				throw std::runtime_error("Failed to write pixels from source image to a frame.");

			if (FAILED(frame->Commit()))
				throw std::runtime_error("Failed to commit a frame.");
		}
		catch (const std::exception &ex)
		{
			SafeRelease(frame);
			throw ex;	// Re-throw the exception.
		}

		SafeRelease(frame);

		return true;
	}

	bool WicFile::Close(void)
	{
		if (this->fileMode == FileMode::Write)
		{
			if (FAILED(this->encoder->Commit()))
				throw std::runtime_error("Failed to commit a file.");
		}
		SafeRelease(this->decoder);
		SafeRelease(this->encoder);
		SafeRelease(this->stream);

		this->isOpened = false;
		return true;
	}
}