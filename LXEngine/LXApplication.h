//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXApplicationBase.h"

class LXWindow;

class LXCORE_API LXApplication : public LXApplicationBase
{

public:

	LXApplication(LXWindow*);
	virtual ~LXApplication();

private:

	void Run() override;
};

