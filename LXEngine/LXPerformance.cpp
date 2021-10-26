//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "pch.h"
#include "LXPerformance.h"

LXPerformance::LXPerformance(void)
{
	Reset();
}

LXPerformance::~LXPerformance(void)
{
}

void LXPerformance::Reset( )
{
	QueryPerformanceFrequency(&m_frequency);
	QueryPerformanceCounter(&m_counter);
}

double LXPerformance::GetTime() const
{
	LARGE_INTEGER li;
	::QueryPerformanceCounter(&li);
	LONGLONG lDelta = li.QuadPart - m_counter.QuadPart;
	double dTime = 1000. * (double)lDelta / (double)m_frequency.QuadPart;
	return dTime;
}

LONGLONG LXPerformance::GetCycles() const
{
	LARGE_INTEGER li;
	::QueryPerformanceCounter(&li);
	return li.QuadPart - m_counter.QuadPart;
}