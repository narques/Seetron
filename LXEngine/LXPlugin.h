//----------------------------------------------------------------------------------////------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXCore/LXObject.h"

class LXProject;
class LXScene;

class LXENGINE_API LXPlugin 
{

public:

	LXPlugin(void);
	virtual ~LXPlugin(void);
	virtual LXScene* Load( LXString strFilename, LXProject* pDocument ) = 0;
	virtual void Release( )=0;
};
