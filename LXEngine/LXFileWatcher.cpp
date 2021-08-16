//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "LXFileWatcher.h"
#include "LXCore.h"
#include "LXDirectory.h"
#include "LXMessageManager.h"
#include "LXMutex.h"

// Use the ReadDirectoryChanges function instead of FindFirstChangeNotification/FindNextChangeNotification 
#define LX_USE_RDC 1

namespace
{
	const LXString kFileModifiedEvent = L"LastWriteFileChanged";
	const LXString kFileAddedEvent = L"FileAdded";
	const LXString kFileRemovedEvent = L"FileRemoved";
}

LXFileWatcher::LXFileWatcher(const wchar_t* pathName, bool watchSubtree)
{
#if LX_USE_RDC
	_handle = CreateFile(
		pathName,
		FILE_LIST_DIRECTORY,
		FILE_SHARE_READ | FILE_SHARE_DELETE,
		NULL,
		OPEN_EXISTING,
		FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
		NULL);
#else
	const DWORD notifyFilter = FILE_NOTIFY_CHANGE_LAST_WRITE;
	_handle = FindFirstChangeNotification(pathName, watchSubtree ? TRUE : FALSE, notifyFilter);
	_directory = make_unique<LXDirectory>(pathName);
#endif

	_mutex = std::make_unique<LXMutex>();

	_channelOnFileChanged = GetMessageManager()->Connect(this, kFileModifiedEvent, this, [this]()
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

	_channelOnFileAdded = GetMessageManager()->Connect(this, kFileAddedEvent, this, [this]()
	{
		if (_onFileAdded)
		{
			_mutex->Lock();
			for (const std::wstring& filename : _fileAdded)
			{
				_onFileAdded(filename);
			}
			_fileAdded.clear();
			_mutex->Unlock();
		}
	});

	_channelOnFileRemoved = GetMessageManager()->Connect(this, kFileRemovedEvent, this, [this]()
	{
		if (_onFileRemoved)
		{
			_mutex->Lock();
			for (const std::wstring& filename : _fileRemoved)
			{
				_onFileRemoved(filename);
			}
			_fileRemoved.clear();
			_mutex->Unlock();
		}
	});
	   	   
	_run = true;
	_thread = new std::thread(LXFileWatcher::Run, this, pathName);
}

LXFileWatcher::~LXFileWatcher()
{
	_run = false;
#if LX_USE_RDC
	CancelIoEx(_handle, nullptr);
	_thread->join();
	CloseHandle(_handle);
#else
	FindCloseChangeNotification(_handle);
	_thread->join();
#endif

	GetMessageManager()->Disconnect(_channelOnFileChanged);
	GetMessageManager()->Disconnect(_channelOnFileAdded);
	GetMessageManager()->Disconnect(_channelOnFileRemoved);

	delete _thread;
}

void LXFileWatcher::OnFileChanded(std::function<void(const std::wstring& filename)> function)
{
	CHK(_onFileChanged == nullptr);
	_onFileChanged = function;
}

void LXFileWatcher::OnFileAdded(std::function<void(const std::wstring& filename)> function)
{
	CHK(_onFileAdded == nullptr);
	_onFileAdded = function;
}

void LXFileWatcher::OnFileRemoved(std::function<void(const std::wstring& filename)> function)
{
	CHK(_onFileRemoved == nullptr);
	_onFileRemoved = function;
}

void LXFileWatcher::Process()
{
#if !LX_USE_RDC

	// WatcherThread

	std::list<std::wstring> addedFiles;
	std::list<std::wstring> removedFiles;
	std::list<std::wstring> modifiedFiles;
	
	_directory->GetChangedLastWriteFiles(addedFiles, removedFiles, modifiedFiles, true);

	if (addedFiles.size())
	{
		//LogD(FileWatcher, L"%i ADDED: %s", actionIndex, fineNotifyInformation->FileName);
		_mutex->Lock();
		_fileAdded.insert(addedFiles.begin(), addedFiles.end());
		_mutex->Unlock();
		GetMessageManager()->Post(_channelOnFileAdded);
	}

	if (removedFiles.size())
	{
		//LogD(FileWatcher, L"%i REMOVED: %s", actionIndex, fineNotifyInformation->FileName);
		_mutex->Lock();
		_fileRemoved.insert(removedFiles.begin(), removedFiles.end());
		_mutex->Unlock();
		GetMessageManager()->Post(_channelOnFileRemoved);
	}

	if (modifiedFiles.size())
	{
		//LogD(FileWatcher, L"%i MODIFIED: %s", actionIndex, fineNotifyInformation->FileName);
		_mutex->Lock();
		_fileChanged.insert(modifiedFiles.begin(), modifiedFiles.end());
		_mutex->Unlock();
		GetMessageManager()->Post(_channelOnFileChanged);
	}

#endif
}

#if LX_USE_RDC

void LXFileWatcher::Run(LXFileWatcher* fw, const wchar_t* pathName)
{
	FILE_NOTIFY_INFORMATION fileNotifyInformations[1024] = {};
	DWORD bytesReturned;

	OVERLAPPED overlapped = {};
	//overlapped.hEvent = CreateEvent(NULL, TRUE, FALSE, nullptr);
	//fw->_event = overlapped.hEvent;

	while (ReadDirectoryChangesW(
		fw->_handle,
		&fileNotifyInformations,
		sizeof(fileNotifyInformations),
		TRUE,
		FILE_NOTIFY_CHANGE_SECURITY |
		FILE_NOTIFY_CHANGE_CREATION |
		FILE_NOTIFY_CHANGE_LAST_ACCESS |
		FILE_NOTIFY_CHANGE_LAST_WRITE |
		FILE_NOTIFY_CHANGE_SIZE |
		FILE_NOTIFY_CHANGE_ATTRIBUTES |
		FILE_NOTIFY_CHANGE_DIR_NAME |
		FILE_NOTIFY_CHANGE_FILE_NAME,
		&bytesReturned,
		NULL,
		NULL))
	{
		if (bytesReturned)
		{
			FILE_NOTIFY_INFORMATION* fineNotifyInformation = &fileNotifyInformations[0];

			//LogD(FileWatcher, L"--- Directorty Changed ----");
			
			int actionIndex = 0;

			while (1)
			{

				const int strLength = fineNotifyInformation->FileNameLength / 2;
				std::wstring filename = std::wstring(fineNotifyInformation->FileName, strLength);

				switch (fineNotifyInformation->Action)
				{
				case FILE_ACTION_ADDED:
				{
					//LogD(FileWatcher, L"%i ADDED: %s", actionIndex, fineNotifyInformation->FileName);
					fw->_mutex->Lock();
					//Size in byte to sting length 
					fw->_fileAdded.insert(filename);
					fw->_mutex->Unlock();
					GetMessageManager()->Post(fw->_channelOnFileAdded);
				}
				break;

				case FILE_ACTION_REMOVED:
				{
					//LogD(FileWatcher, L"%i REMOVED: %s", actionIndex, fineNotifyInformation->FileName);
					fw->_mutex->Lock();
					fw->_fileRemoved.insert(filename);
					fw->_mutex->Unlock();
					GetMessageManager()->Post(fw->_channelOnFileRemoved);
				}
				break;

				case FILE_ACTION_MODIFIED:
				{
					//LogD(FileWatcher, L"%i MODIFIED: %s", actionIndex, fineNotifyInformation->FileName);
					fw->_mutex->Lock();
					fw->_fileChanged.insert(filename);
					fw->_mutex->Unlock();
					GetMessageManager()->Post(fw->_channelOnFileChanged);
				}
				break;

				case FILE_ACTION_RENAMED_OLD_NAME:
				{
					//LogD(FileWatcher, L"%i RENAMED (OLD): %s", actionIndex, fineNotifyInformation->FileName);
				}
				break;

				case  FILE_ACTION_RENAMED_NEW_NAME:
				{
					//LogD(FileWatcher, L"%i RENAMED (NEW): %s", actionIndex, fineNotifyInformation->FileName);
				}
				break;

				default:
					CHK(0);
					break;
				
				} // Switch

				if (fineNotifyInformation->NextEntryOffset > 0)
				{
					unsigned char* ptr = (unsigned char*)fineNotifyInformation;
					ptr += fineNotifyInformation->NextEntryOffset;
					fineNotifyInformation = (FILE_NOTIFY_INFORMATION*)ptr;
					actionIndex++;
				}
				else
				{
					break;
				}
			}
		}
	}
}

#else

void LXFileWatcher::Run(LXFileWatcher* fw, const wchar_t* pathName)
{
	while (fw->_run)
	{
		if (WaitForSingleObject(fw->_handle, INFINITE) == WAIT_OBJECT_0)
		{
			if (!fw->_run)
				break;

			fw->Process();

			FindNextChangeNotification(fw->_handle);
		}
	}
}

#endif
