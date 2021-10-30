//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXCore/LXObject.h"
#include <windows.h>

class LXTime : public LXObject
{

public:

	LXTime();
	virtual ~LXTime();

	void	Update();
	double	Time()		 const { return _previousTime; }
	double	DeltaTime()  const { return _deltaTime; }
	int		FrameCount() const { return _frameCount; }

	double GetFPS() const { return _FPS; }

private:

	double	GetTime() const;

private:

	LARGE_INTEGER m_frequency;
	LARGE_INTEGER m_counter;

	double _previousTime = 0.;
	double _deltaTime = 0.;
	uint _frameCount = 0;
	
	double _AccumulatedTime = 0.f;
	double _FPS = 0.;
	
};

