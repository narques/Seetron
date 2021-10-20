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

LXENGINE_API extern DWORD MainThread;
LXENGINE_API extern DWORD RenderThread;
LXENGINE_API extern DWORD LoadingThread;
LXENGINE_API extern bool IsSyncPoint;

static bool IsMainThread()	{ return ::GetCurrentThreadId() == MainThread; }
static bool IsRenderThread() { return ::GetCurrentThreadId() == RenderThread; }
static bool IsLoadingThread() { return ::GetCurrentThreadId() == LoadingThread; }

