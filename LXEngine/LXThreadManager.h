//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXObject.h"

class LXThreadManager
{

public:

	LXThreadManager();
	~LXThreadManager();
	
};

LXCORE_API extern DWORD MainThread;
LXCORE_API extern DWORD RenderThread;
LXCORE_API extern DWORD LoadingThread;
LXCORE_API extern bool IsSyncPoint;

static bool IsMainThread()	{ return ::GetCurrentThreadId() == MainThread; }
static bool IsRenderThread() { return ::GetCurrentThreadId() == RenderThread; }
static bool IsLoadingThread() { return ::GetCurrentThreadId() == LoadingThread; }

