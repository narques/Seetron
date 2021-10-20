//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXObject.h"

class LXENGINE_API LXSyncEvent : public LXObject
{
public:

	LXSyncEvent(bool initialState);
	virtual ~LXSyncEvent();

	void SetEvent();
	void Wait();
	void Reset();

private:

	HANDLE _handle = nullptr;

};

