//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXPlatform.h"

class LXENGINE_API LXPoint
{

public:

	LXPoint(void);
	LXPoint(int nx, int ny) { x = nx; y = ny; }
	~LXPoint(void);

	LXPoint(const LXPoint& point)
	{
		x = point.x;
		y = point.y;
	}

	long  x;
	long  y;

};
