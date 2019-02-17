//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2019 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "LXDirectory.h"
#include <windows.h>
#include "LXMemory.h" // --- Must be the last included ---

LX_PRAGMA_OPTIM_OFF

LXDirectory::LXDirectory(const wchar_t* path)
{
	std::wstring stdFolder = path;
	
	// If path not a regular expression create a default one.
	if ((stdFolder.find(L'*') == string::npos) &&
		(stdFolder.find(L'?') == string::npos))

	{
		_BaseFolder = path;
		
		std::wstring regExpression = path;
		wchar_t lastCharacter = regExpression.back();
		
		if (lastCharacter != L'/' && 
			lastCharacter != '\\')
		{
			regExpression.push_back(L'/');
		}

		regExpression.push_back(L'*');
		ScanFolder(regExpression);
	}
	else
	{
		ScanFolder(path);
	}
}

LXDirectory::~LXDirectory()
{
	_listFiles.clear();
}

void LXDirectory::ScanFolder(const std::wstring& folder)
{
	WIN32_FIND_DATA fd;
	HANDLE hFind = FindFirstFile(folder.c_str(), &fd);

	if (hFind != INVALID_HANDLE_VALUE)
	{
		if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		{
			AddFillInfo(folder, fd);
		}

		while (FindNextFile(hFind, &fd))
		{
			if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				if ((wcscmp(fd.cFileName, L".") != 0) && (wcscmp(fd.cFileName, L"..") != 0))
				{
					std::wstring newFolder = folder;
					newFolder = newFolder.substr(0, newFolder.length() - 1);
					newFolder += fd.cFileName;
					newFolder += L"/*";
					ScanFolder(newFolder.c_str());
				}
			}
			else
			{
				AddFillInfo(folder, fd);
			}
		}
		FindClose(hFind);
	}
}

void LXDirectory::AddFillInfo(const std::wstring& folder, const WIN32_FIND_DATA& findData)
{
	LXFileInfo fi;
	fi.FileName = findData.cFileName;
	fi.LocalFileName = folder.substr(0, folder.length() - 1);
	fi.FullFileName = fi.LocalFileName + fi.FileName;
	fi.LocalFileName = fi.LocalFileName.substr(_BaseFolder.length(), fi.LocalFileName.length() - _BaseFolder.length()) + fi.FileName;
	fi.LastWriteTime = findData.ftLastWriteTime;
	_listFiles.push_back(fi);
}

bool LXDirectory::Exist() const
{
	DWORD dwAttrib = GetFileAttributes(_BaseFolder.c_str());
	return (dwAttrib != INVALID_FILE_ATTRIBUTES && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

bool LXDirectory::IsEmpty() const
{
	WIN32_FIND_DATA fd;
	HANDLE hFind = FindFirstFile(_BaseFolder.c_str(), &fd);
	return false;
}

bool LXDirectory::Exists(wchar_t* folder)
{
	DWORD dwAttrib = GetFileAttributes(folder); 
	return (dwAttrib != INVALID_FILE_ATTRIBUTES && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

void LXDirectory::GetChangedLastWriteFiles(list<std::wstring>& filenames, bool update /*=true*/)
{
	for (LXFileInfo& fileInfo : _listFiles)
	{
		WIN32_FIND_DATA fd;
		HANDLE hFind = FindFirstFile(fileInfo.FullFileName.c_str(), &fd);
		
		if ((fileInfo.LastWriteTime.dwHighDateTime != fd.ftLastWriteTime.dwHighDateTime) ||
		   (fileInfo.LastWriteTime.dwLowDateTime != fd.ftLastWriteTime.dwLowDateTime))
		{
			filenames.push_back(fileInfo.FullFileName);
			if (update)
			{
				fileInfo.LastWriteTime = fd.ftLastWriteTime;
			}
		}
	}
}

LX_PRAGMA_OPTIM_ON