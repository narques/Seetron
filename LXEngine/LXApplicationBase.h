//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXPlatform.h"

class LXWindow;

class LXENGINE_API LXApplicationBase
{

public:

	LXApplicationBase(LXWindow*);
	virtual ~LXApplicationBase();

	int run();

private:

	virtual void Run()=0;

protected:

	LXWindow* _windowBase = nullptr;

};

