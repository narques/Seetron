//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2019 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXTask.h"
#include <thread>

class LXDirectory;

class LXCORE_API LXFileWatcher
{

public:

	LXFileWatcher(const wchar_t* pathName, bool watchSubtree = false);
	~LXFileWatcher();

	void OnFileChanded(std::function<void(const std::wstring& filename)> function);
	
private:

	static void Run(LXFileWatcher*, const wchar_t* pathName);

	HANDLE _handle = 0;
	set<std::wstring> _fileChanged;
	std::thread* _thread;
	bool _run = true;
	std::function<void(const std::wstring& filename)> _onFileChanged;
	unique_ptr<LXDirectory> _directory;
	unique_ptr <LXMutex> _mutex;
};

