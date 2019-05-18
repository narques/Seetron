//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXObject.h"
#include "LXTask.h"
#include <thread>

struct LXChannel;
class LXDirectory;

class LXCORE_API LXFileWatcher : public LXObject
{

public:

	LXFileWatcher(const wchar_t* pathName, bool watchSubtree = false);
	~LXFileWatcher();

	void OnFileChanded(std::function<void(const std::wstring& filename)> function);
	void OnFileAdded(std::function<void(const std::wstring& filename)> function);
	void OnFileRemoved(std::function<void(const std::wstring& filename)> function);
	
private:

	static void Run(LXFileWatcher*, const wchar_t* pathName);
	void Process();

	HANDLE _handle = 0;
	//HANDLE _event = 0;

	set<std::wstring> _fileChanged;
	set<std::wstring> _fileAdded;
	set<std::wstring> _fileRemoved;

	std::thread* _thread;
	bool _run = true;

	std::function<void(const std::wstring& filename)> _onFileChanged;
	std::function<void(const std::wstring& filename)> _onFileAdded;
	std::function<void(const std::wstring& filename)> _onFileRemoved;

	const LXChannel* _channelOnFileChanged;
	const LXChannel* _channelOnFileAdded;
	const LXChannel* _channelOnFileRemoved;
	
	unique_ptr<LXDirectory> _directory;
	unique_ptr<LXMutex> _mutex;
};

