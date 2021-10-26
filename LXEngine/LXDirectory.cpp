//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "pch.h"
#include "LXDirectory.h"
#include <windows.h>

std::wstring GetFolderFromPartialFilepath(const std::wstring& partialFilepath)
{
	if ((partialFilepath.find(L'*') != std::string::npos) ||
		(partialFilepath.find(L'?') != std::string::npos))
	{
		size_t pos = partialFilepath.rfind(L"\\");
		if (pos == std::wstring::npos)
		{
			pos = partialFilepath.rfind(L"/");
		}

		if (pos != std::wstring::npos)
		{
			return partialFilepath.substr(0, pos + 1);
		}
		else
		{
			CHK(0);
			return L"";
		}
	}
	else
	{
		return partialFilepath;
	}
}

LXDirectory::LXDirectory(const wchar_t* path)
{
	std::wstring stdFolder = path;
	
	if ((stdFolder.find(L'*') != std::string::npos) ||
		(stdFolder.find(L'?') != std::string::npos))
	{
		_searchRegExpression = path;

		size_t pos =  _searchRegExpression.rfind(L"\\");
		if (pos == std::wstring::npos)
		{
			pos = _searchRegExpression.rfind(L"/");
		}
		
		if (pos != std::wstring::npos)
		{
			_baseFolder = _searchRegExpression.substr(0, pos + 1);
		}
		else
		{
			CHK(0);
		}
	}
	else
	{
		// If path not a regular expression create a default one.
		_baseFolder = path;

		_searchRegExpression = path;
		
		wchar_t lastCharacter = _searchRegExpression.back();
		if (lastCharacter != L'/' && 
			lastCharacter != '\\')
		{
			_searchRegExpression.push_back(L'/');
		}

		_searchRegExpression.push_back(L'*');
	}
	
	ScanFolder(_searchRegExpression, true, nullptr);
}

LXDirectory::~LXDirectory()
{
	_listFiles.clear();
}

void LXDirectory::ScanFolder(const std::wstring& folder, bool update, std::list<std::wstring>* outAddedFiles)
{
	WIN32_FIND_DATA fd;
	HANDLE hFind = FindFirstFile(folder.c_str(), &fd);

	if (hFind != INVALID_HANDLE_VALUE)
	{
		if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		{
			AddFillInfo(folder, fd, update, outAddedFiles);
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
					ScanFolder(newFolder.c_str(), update, outAddedFiles);
				}
			}
			else
			{
				AddFillInfo(folder, fd, update, outAddedFiles);
			}
		}
		FindClose(hFind);
	}
}

void LXDirectory::AddFillInfo(const std::wstring& folder, const WIN32_FIND_DATA& findData, bool update, std::list<std::wstring>* outAddedFiles)
{
	std::wstring absoluteFolder = GetFolderFromPartialFilepath(folder);
	   
	LXFileInfo fi;
	fi.FileName = findData.cFileName;
	fi.FullFileName = absoluteFolder + fi.FileName;
	fi.LastWriteTime = findData.ftLastWriteTime;
	
	// Check if exist
	auto it = std::find_if(_listFiles.begin(), _listFiles.end(), [fi](LXFileInfo& fileIndo)
	{
		return fi.FullFileName.compare(fileIndo.FullFileName) == 0;
	});

	if (it == _listFiles.end())
	{
		if (update)
		{
			_listFiles.push_back(fi);
		}
		
		if (outAddedFiles)
		{
			outAddedFiles->push_back(fi.FullFileName);
		}
	}
}

bool LXDirectory::Exist() const
{
	DWORD dwAttrib = GetFileAttributes(_baseFolder.c_str());
	return (dwAttrib != INVALID_FILE_ATTRIBUTES && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

bool LXDirectory::IsEmpty() const
{
	WIN32_FIND_DATA fd;
	HANDLE hFind = FindFirstFile(_baseFolder.c_str(), &fd);
	return false;
}

bool LXDirectory::Exists(const wchar_t* folder)
{
	DWORD dwAttrib = GetFileAttributes(folder); 
	return (dwAttrib != INVALID_FILE_ATTRIBUTES && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

void LXDirectory::GetChangedLastWriteFiles(std::list<std::wstring>& addedFiles, std::list<std::wstring>& removedFiles, std::list<std::wstring>& updatedFiles, bool update /*=true*/)
{
	for (LXFileInfo& fileInfo : _listFiles)
	{
		WIN32_FIND_DATA fd;
		
		HANDLE hFind = FindFirstFile(fileInfo.FullFileName.c_str(), &fd);

		if (hFind == INVALID_HANDLE_VALUE)
		{
			// Removed file
			removedFiles.push_back(fileInfo.FullFileName);
		}
		else
		{
			// Modified file (based on the Last write time)
			if ((fileInfo.LastWriteTime.dwHighDateTime != fd.ftLastWriteTime.dwHighDateTime) ||
				(fileInfo.LastWriteTime.dwLowDateTime != fd.ftLastWriteTime.dwLowDateTime))
			{
				updatedFiles.push_back(fileInfo.FullFileName);
				
				if (update)
				{
					fileInfo.LastWriteTime = fd.ftLastWriteTime;
				}
			}

			FindClose(hFind);
		}
	}

	// Added file
	ScanFolder(_searchRegExpression, update, &addedFiles);
}
