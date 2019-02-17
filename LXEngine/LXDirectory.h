//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2019 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXPlatform.h"
#include <list>
#include <string>

struct LXFileInfo
{
	std::wstring FileName;
	std::wstring LocalFileName;
	std::wstring FullFileName;
	FILETIME LastWriteTime;
};

typedef std::list<LXFileInfo> ListFileInfos;

class LXCORE_API LXDirectory
{

public:

	LXDirectory(const wchar_t* path);
	~LXDirectory();
	const ListFileInfos& GetListFileNames() const { return _listFiles; }
	bool Exist() const;
	bool IsEmpty() const;
	static bool Exists(wchar_t* folder);
	
	// Compares the cached LastWriteTime again the real value,
	// returns the filenames when different.
	// updates the cached value.

	void GetChangedLastWriteFiles(list<std::wstring>& filenames, bool update = true);

private:

	void ScanFolder(const std::wstring& folder);
	void AddFillInfo(const std::wstring& folder, const WIN32_FIND_DATA& fd);
	
private:

	std::wstring _BaseFolder;
	ListFileInfos _listFiles;
};
