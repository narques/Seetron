//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXCore/LXObject.h"
#include "LXPlatform.h"

class LXThread : public LXObject
{

public:

	LXThread();
	virtual ~LXThread();

	void Run(void* lpStartAddr, void* param);
	void Wait();

	void Suspend();
	void Resume();

	void GetInformation();

private:

	void* _handle = nullptr;
};
