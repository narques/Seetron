//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "LXApplication.h"
#include "LXCore.h"

LXApplication::LXApplication(LXWindow* windowBase):LXApplicationBase(windowBase)
{
}

LXApplication::~LXApplication()
{
}

void LXApplication::Run()
{
	GetCore().Run();
}
