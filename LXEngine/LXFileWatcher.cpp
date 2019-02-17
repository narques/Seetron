//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2019 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "LXFileWatcher.h"
#include "LXCore.h"
#include "LXDirectory.h"
#include "LXEvent.h"
#include "LXEventManager.h"
#include "LXThread.h"

namespace
{
	const LXString kLastWriteFileChangedEvent = L"LastWriteFileChanged";
}

LXFileWatcher::LXFileWatcher(const wchar_t* pathName, bool watchSubtree)
{
	DWORD notifyFilter = FILE_NOTIFY_CHANGE_LAST_WRITE;
	_handle = FindFirstChangeNotification(pathName, watchSubtree ? TRUE : FALSE, notifyFilter);
	_directory = make_unique<LXDirectory>(pathName);
	_mutex = make_unique<LXMutex>();

	GetEventManager()->RegisterEventFunc(kLastWriteFileChangedEvent, this, [this](LXEvent*)
	{
		if (_onFileChanged)
		{
			_mutex->Lock();
			for (const std::wstring& filename: _fileChanged)
			{
				_onFileChanged(filename);
			}
			_fileChanged.clear();
			_mutex->Unlock();
		}
	});
	
	_run = true;
	_thread = new std::thread(LXFileWatcher::Run, this, pathName);
}

LXFileWatcher::~LXFileWatcher()
{
	_run = false;
	FindCloseChangeNotification(_handle);
	_thread->join();
	delete _thread;
}

void LXFileWatcher::OnFileChanded(std::function<void(const std::wstring& filename)> function)
{
	_onFileChanged = function;
}

void LXFileWatcher::Run(LXFileWatcher* fw, const wchar_t* pathName)
{
	while (fw->_run)
	{
		if (WaitForSingleObject(fw->_handle, INFINITE) == WAIT_OBJECT_0)
		{
			if (!fw->_run)
				break;

			list<std::wstring> filenames;
			fw->_directory->GetChangedLastWriteFiles(filenames);

			for (const std::wstring& filename : filenames)
			{
				fw->_mutex->Lock();
				fw->_fileChanged.insert(filename);
				fw->_mutex->Unlock();
				GetEventManager()->PostEvent(kLastWriteFileChangedEvent);
			}
			
			FindNextChangeNotification(fw->_handle);
		}
	}
}
