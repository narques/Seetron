//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "LXStatistic.h"
#include "LXEngine.h"
#include "LXLogger.h"
#include "LXStatManager.h"

#ifdef LX_COUNTERS

LXCounter::LXCounter(const LXString& expression):_expression(expression)
{
	GetEngine().GetCounters().push_back(this);
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

LXPerformanceLogger::LXPerformanceLogger(const wchar_t* name):_name(name)
{
}

LXPerformanceLogger::~LXPerformanceLogger()
{
	double elapsedTime = GetTime();
	LogD(Performance, L"%s : %f", _name.c_str(), elapsedTime);
}

LXPerformanceScope::LXPerformanceScope(const wchar_t* name):_name(name)
{
	if (GetStatManager())
	{
		GetStatManager()->OpenStat(_name);
	}
}

LXPerformanceScope::~LXPerformanceScope()
{
	double elapsedTime = GetTime();
	if (GetStatManager())
	{
		GetStatManager()->UpdateAndCloseStat(_name, elapsedTime);
	}
}

void LXPerformanceScope::Update()
{
	double elapsedTime = GetTime();
	GetStatManager()->UpdateAndCloseStat(_name, elapsedTime);
	Reset();
	GetStatManager()->OpenStat(_name);
}

LXCountScopeIncrement::LXCountScopeIncrement(const wchar_t* name)
{
	GetStatManager()->UpdateCounter(name, 1);
}

LXCountScopeDecrement::LXCountScopeDecrement(const wchar_t* name)
{
	GetStatManager()->UpdateCounter(name, -1);
}
