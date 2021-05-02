//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXPlatform.h"
#include <list>
#include <string>

struct LXFileInfo
{
	std::wstring FileName;
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
	static bool Exists(const wchar_t* folder);
	
	// Compares the cached LastWriteTime again the real value,
	// returns the filenames when different.
	// updates the cached value.

	void GetChangedLastWriteFiles(std::list<std::wstring>& addedFiles, std::list<std::wstring>& removedFiles, std::list<std::wstring>& updatedFiles, bool update = true);

private:

	void ScanFolder(const std::wstring& folder, bool update, std::list<std::wstring>* outAddedFiles);
	void AddFillInfo(const std::wstring& folder, const WIN32_FIND_DATA& fd, bool update, std::list<std::wstring>* outAddedFiles);
	
private:

	std::wstring _searchRegExpression;
	std::wstring _baseFolder;
	ListFileInfos _listFiles;
};
