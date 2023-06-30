#include "FileDialog.h"

#include <Windows.h>
#include <commdlg.h>

namespace Zenit {
	std::string FileDialog::OpenFile(const char* filter)
	{
		OPENFILENAME file;
		char size[512] = { 0 };
		ZeroMemory(&file, sizeof(OPENFILENAME));
		file.lStructSize = sizeof(OPENFILENAME);

		file.hwndOwner = GetActiveWindow();
		file.lpstrFile = size;
		file.nMaxFile = sizeof(size);
		file.lpstrFilter = filter;
		file.nFileExtension = (WORD)".zenit";
		file.nFilterIndex = 1;
		file.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
		if (GetOpenFileName(&file))
		{
			return file.lpstrFile;
		}

		return std::string();
	}

	std::string FileDialog::SaveFile(const char* filter)
	{
		OPENFILENAME file;
		char size[512] = { 0 };
		ZeroMemory(&file, sizeof(OPENFILENAME));
		file.lStructSize = sizeof(OPENFILENAME);

		file.hwndOwner = GetActiveWindow();
		file.lpstrFile = size;
		file.nMaxFile = sizeof(size);
		file.lpstrFilter = filter;
		file.nFileExtension = (WORD)".zenit";
		file.nFilterIndex = 1;
		file.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
		if (GetSaveFileName(&file))
		{
			return file.lpstrFile;
		}

		return std::string();
	}

}