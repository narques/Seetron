//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

class LXENGINE_API LXMutex
{

public:

	LXMutex();
	virtual ~LXMutex();

	bool Lock(/*DWORD*/ unsigned long dwTimeOut = 0xFFFFFFFF/*INFINITE*/);
	bool Unlock();

private:

#if LX_WINDOWS_MUTEX
	void* _hMutex = nullptr;
#else
	std::mutex _mutex;
#endif
};