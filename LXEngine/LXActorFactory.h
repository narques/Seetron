//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------*

#pragma once

#include "LXObject.h"

class LXActorFactory : public LXObject
{

public:

	LXActorFactory();
	virtual ~LXActorFactory();

	LXActor* CreateActor(const LXString& ClassName);
};

