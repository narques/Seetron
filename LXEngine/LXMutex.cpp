//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "pch.h"
#include "LXMutex.h"

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