//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "LXThread.h"
#include "targetver.h"
#include <windows.h>

LXThread::LXThread() 
{
}

LXThread::~LXThread()
{
}

void LXThread::Run(void* lpStartAddr, void* param)
{
	_handle = ::CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)lpStartAddr, param, 0, NULL);
}

void LXThread::Wait()
{
	::WaitForSingleObject(_handle, INFINITE);
}

void LXThread::Suspend()
{
	DWORD SuspendCount = ::SuspendThread(_handle);
}

void LXThread::Resume()
{
	DWORD SuspendCount = ::ResumeThread(_handle);
}

void LXThread::GetInformation()
{
	MEMORY_PRIORITY_INFORMATION MemPrio;
	::GetThreadInformation(_handle, ThreadMemoryPriority, &MemPrio, sizeof(MemPrio));
}

//----------------------------------------------------------------------------------------------------------

LXSyncEvent::LXSyncEvent(const wchar_t* Name, BOOL InitialState)
{
	_handle = ::CreateEvent(NULL, TRUE, InitialState, Name);
}

LXSyncEvent::~LXSyncEvent()
{
	::CloseHandle(_handle);
}

void LXSyncEvent::SetEvent()
{
	::SetEvent(_handle);
}

void LXSyncEvent::Wait()
{
	::WaitForSingleObject(_handle, INFINITE);
}

void LXSyncEvent::Reset()
{
	::ResetEvent(_handle);
}

//----------------------------------------------------------------------------------------------------------

LXMutex::LXMutex()
{
#if LX_WINDOWS_MUTEX
	_hMutex = ::CreateMutex(NULL, FALSE, NULL); 
#endif
}

LXMutex::~LXMutex()
{
#if LX_WINDOWS_MUTEX
	::CloseHandle(_hMutex);
#endif
}

bool LXMutex::Lock(unsigned long dwTimeOut)
{
#if LX_WINDOWS_MUTEX
	WaitForSingleObject(_hMutex, INFINITE);
#else
	_mutex.lock();
#endif
	return true;
}

bool LXMutex::Unlock()
{
#if LX_WINDOWS_MUTEX
	ReleaseMutex(_hMutex);
#else
	_mutex.unlock();
#endif
	return true;
}

//----------------------------------------------------------------------------------------------------------

LXSemaphore::LXSemaphore(const wchar_t* Name)
{
	_hSemaphore = ::CreateSemaphore(NULL, 3, 3, Name);
}

LXSemaphore::~LXSemaphore()
{

}
