#pragma once


#include <windows.h>
#include <shlobj.h>
#include <string>


namespace bb {

/*
	This header file provides a adhoc system to store game data into Local Appdata folder

	Here's a description of Local AppData folder:

	The Local AppData folder (C:\Users\<username>\AppData\Local) is a user-specific, non-synced
	directory for storing machine-specific data like cache files, logs, or game saves that don’t
	need to move between devices. It’s ideal for games because it doesn’t require elevated
	permissions, isolates data for each user, and avoids unnecessary syncing, making it perfect
	for storing large or hardware-dependent save files and temporary data.
*/

// Function to get the path to Local AppData folder

std::wstring get_local_appdata_path()
{	
	std::wstring var;

	wchar_t *path;

	/*
		to know more about "SHGetKnownFolderPath",

		visit: https://learn.microsoft.com/en-us/windows/win32/api/shlobj_core/nf-shlobj_core-shgetknownfolderpath

		"FOLDERID_LocalAppData" is the KNOWNFOLDERID for Local AppData folder, to know more,

		visit: https://learn.microsoft.com/en-us/windows/win32/shell/csidl
	*/

	if (SHGetKnownFolderPath(FOLDERID_LocalAppData, KF_FLAG_DEFAULT, NULL, &path) == S_OK)
	{
		var = path;
	}

	CoTaskMemFree(path);

	return var;
}

/*
	takes relative path to a file inside Local AppData Folder, to read, and a reference to a data
	(preferable a structure), to load the read data

	returns true -> success, false -> failure
*/

template<typename struct_data>

bool load_local_appdata(const std::string& path, struct_data& data)
{
	bool return_value = false;

	FILE* file = NULL;

	file = _wfopen((get_local_appdata_path() + L"\\" + std::wstring(path.cbegin(), path.cend())).c_str(), L"rb");

	if (file)
	{
		// file found

		if(fread(&data, sizeof(data), 1, file) == 1);	// read ths object out ofthe file
		{
			return_value = true;
		}

		fclose(file);
	}

	return return_value;
}

/*
	takes relative path to the file being stored inside Local AppData Folder and a reference to the
	data being stored

	returns true -> success, false -> failure
*/

template<typename struct_data>

bool store_local_appdata(const std::string& path, struct_data& data)
{
	bool return_value = false;

	FILE* file = NULL;

	file = _wfopen((get_local_appdata_path() + L"\\" + std::wstring(path.cbegin(), path.cend())).c_str(), L"wb");

	if (file)
	{
		// file found

		if (fwrite(&data, sizeof(data), 1, file) == 1);	// read ths object out ofthe file
		{
			return_value = true;
		}

		fclose(file);
	}

	return return_value;
}

} // namespace bb