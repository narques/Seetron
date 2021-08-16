//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "LXTime.h"

LXTime::LXTime()
{
	QueryPerformanceFrequency(&m_frequency);
	QueryPerformanceCounter(&m_counter);
}

LXTime::~LXTime()
{
}

double LXTime::GetTime() const
{
	LARGE_INTEGER li;
	QueryPerformanceCounter(&li);
	LONGLONG lDelta = li.QuadPart - m_counter.QuadPart;
	double dTime = 1000. * (double)lDelta / (double)m_frequency.QuadPart;
	return dTime;
}

void LXTime::Update()
{
	_frameCount++;
	
	double currentTime = GetTime();

	_deltaTime = currentTime - _previousTime;
	_previousTime = currentTime;

	_AccumulatedTime += _deltaTime;
	if (_AccumulatedTime > 1000.)
	{
		_FPS = _frameCount;
		_frameCount = 0;
		_AccumulatedTime = 0.;// -= (1000.0 / 4.0);
	}

}