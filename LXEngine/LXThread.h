//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXPlatform.h"

class LXCORE_API LXThread
{

public:

	LXThread();
	~LXThread();

	void Run(void* lpStartAddr, void* param);
	void Wait();

	void Suspend();
	void Resume();

	void GetInformation();

private:

	void* _handle = nullptr;
};

//----------------------------------------------------------------------------------------------------------

class LXCORE_API LXSyncEvent
{
public:

	LXSyncEvent(const wchar_t* Name,/*BOOL*/ int InitialState);
	~LXSyncEvent();
	void SetEvent();
	void Wait();
	void Reset();

private:

	HANDLE _handle = nullptr;
	
};

//----------------------------------------------------------------------------------------------------------

class LXCORE_API LXMutex
{

public:

	LXMutex();
	~LXMutex();

	bool Lock(/*DWORD*/ unsigned long dwTimeOut = 0xFFFFFFFF/*INFINITE*/);
	bool Unlock();

private:
		
#if LX_WINDOWS_MUTEX
	void* _hMutex = nullptr;
#else
	std::mutex _mutex;
#endif
};

//----------------------------------------------------------------------------------------------------------

class LXCORE_API LXSemaphore
{

public:

	LXSemaphore(const wchar_t* Name);
	~LXSemaphore();

private:
	
	void* _hSemaphore = nullptr;

};