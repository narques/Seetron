//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "LXThreadManager.h"

DWORD MainThread = 0;
DWORD RenderThread = 0;
DWORD LoadingThread = 0;
bool IsSyncPoint = false;

LXThreadManager::LXThreadManager()
{
}


LXThreadManager::~LXThreadManager()
{
}
