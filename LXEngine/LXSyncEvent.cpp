//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "LXSyncEvent.h"
#include "LXMemory.h" // --- Must be the last included ---

LXSyncEvent::LXSyncEvent(bool initialState)
{
	_handle = ::CreateEvent(NULL, TRUE, initialState, nullptr);
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
