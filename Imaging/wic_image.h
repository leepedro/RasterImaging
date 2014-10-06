#if !defined(WIC_IMAGE_H)
#define WIC_IMAGE_H

// Windows header files.
//#include <Windows.h>
#include <wincodec.h>

#include "raster_image.h"

namespace Imaging
{
	template <typename I>
	void SafeRelease(I *&rc)
	{
		if (rc != nullptr)
		{
			rc->Release();
			rc = nullptr;
		}
	}

	class WicFile : public RasterFileI
	{
	public:
		WicFile(void);
		~WicFile(void);
		bool Open(const std::wstring &path, FileMode mode = FileMode::Read) override;
		bool Read(RasterImage &imgDst, unsigned int frameNo = 0) override;
		bool Write(const RasterImage &imgDst, unsigned int frameNo = 0) override;
		bool Close(void) override;

	protected:
		static bool GetContainerGUID(const std::wstring &ext, GUID &guid);
		::WICPixelFormatGUID formatGuid;

	private:
		::IWICImagingFactory *factory = nullptr;
		::IWICBitmapDecoder *decoder = nullptr;		// only for reading
		::IWICBitmapEncoder *encoder = nullptr;		// only for writing
		::IWICStream *stream = nullptr;				// only for writing
	};

}

#endif
