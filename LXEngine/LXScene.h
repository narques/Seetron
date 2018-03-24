//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXActor.h"

class LXCORE_API LXScene : public LXActor
{

public:

	LXScene(LXProject* pDocument);
	virtual ~LXScene(void);

	LXActor* GetActor(const LXString& Name);
};
