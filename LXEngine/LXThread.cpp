//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "LXThread.h"
#include "targetver.h"
#include <windows.h>
#include "LXMemory.h" // --- Must be the last included ---

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

