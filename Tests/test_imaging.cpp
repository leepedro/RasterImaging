#include <ShlObj.h>

#include "raster_image.h"
#include "wic_image.h"

void GetFilePath(std::wstring &path)
{
	::IFileOpenDialog *file_dialog(nullptr);
	if (SUCCEEDED(::CoCreateInstance(__uuidof(::FileOpenDialog), nullptr, CLSCTX_ALL, IID_PPV_ARGS(&file_dialog))))
	{
		if (SUCCEEDED(file_dialog->Show(nullptr)))
		{
			::IShellItem *selected_item(nullptr);
			if (SUCCEEDED(file_dialog->GetResult(&selected_item)))
			{
				wchar_t *path_(nullptr);
				if (SUCCEEDED(selected_item->GetDisplayName(::SIGDN_FILESYSPATH, &path_)))
				{
					path = path_;
					::CoTaskMemFree(path_);
				}
				else
					::MessageBoxW(nullptr, L"Failed to get the path of the selected file.", L"Error", MB_OK);
			}
			else
				::MessageBoxW(nullptr, L"Failed to get result from a file dialog.", L"Error", MB_OK);
		}
		else
			::MessageBoxW(nullptr, L"Failed to show a file dialog.", L"Error", MB_OK);

		file_dialog->Release();
	}
	else
		::MessageBoxW(nullptr, L"Failed to create a file dialog.", L"Error", MB_OK);
}

int main(void)
{
	if (SUCCEEDED(::CoInitializeEx(nullptr, ::COINIT_APARTMENTTHREADED | ::COINIT_DISABLE_OLE1DDE)))
	{
		try
		{
			using namespace Imaging;

			WicFile wic_file1;
			std::wstring path;
			GetFilePath(path);
			wic_file1.Open(path, RasterFileI::FileMode::Read);
			RasterImage img1;
			wic_file1.Read(img1);

		}
		catch (const std::exception &ex)
		{
			::MessageBoxA(nullptr, ex.what(), "Error", MB_OK);
		}

		::CoUninitialize();
	}
}