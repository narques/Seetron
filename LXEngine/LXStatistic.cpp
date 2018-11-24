//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "StdAfx.h"
#include "LXStatistic.h"
#include "LXCore.h"
#include "LXStatManager.h"
#include "LXMemory.h" // --- Must be the last included ---

#ifdef LX_COUNTERS

LXCounter::LXCounter(const LXString& expression):_expression(expression)
{
	GetCore().GetCounters().push_back(this);
}

LXCounter::~LXCounter(void)
{
}

LXString LXCounter::GetSentence()
{
	wchar_t Message[256];
	swprintf(Message, 255, _expression.GetBuffer(), _val);
	return Message;
}

#endif

LXPerformanceScope::LXPerformanceScope(const wchar_t* Name):_Name(Name)
{
	GetStatManager()->OpenStat(_Name);
}

LXPerformanceScope::~LXPerformanceScope()
{
	double ElapsedTime = GetTime();
	GetStatManager()->UpdateAndCloseStat(_Name, ElapsedTime);
}

void LXPerformanceScope::Update()
{
	double ElapsedTime = GetTime();
	GetStatManager()->UpdateAndCloseStat(_Name, ElapsedTime);
	Reset();
	GetStatManager()->OpenStat(_Name);
}

LXCountScopeIncrement::LXCountScopeIncrement(const wchar_t* Name)
{
	GetStatManager()->UpdateCounter(Name, 1);
}

LXCountScopeDecrement::LXCountScopeDecrement(const wchar_t* Name)
{
	GetStatManager()->UpdateCounter(Name, -1);
}
