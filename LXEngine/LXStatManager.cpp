//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "LXStatManager.h"
#include "LXMemory.h" // --- Must be the last included ---

LXStatManager::LXStatManager()
{
}

LXStatManager::~LXStatManager()
{
	for(const auto& Counter: Counters)
	{
		CHK(Counter.second == 0);
		if (Counter.second != 0)
		{
			LogW(LXStatManager, L"Counter %s not empty (%i).", Counter.first.c_str(), Counter.second);
		}
	}
}

void LXStatManager::Reset()
{
	for (auto &Stat : Stats)
	{
		Stat.second.Time = 0.0;
		Stat.second.Hits = 0;
	}
}

void LXStatManager::UpdateCounter(const wstring& Name, uint Value)
{
	Counters[Name] += Value;
}

void LXStatManager::UpdateStat(const wstring& Name, double Value)
{
	LXStat& Stat = Stats[Name];
	Stat.Time+= Value;
	Stat.Hits++;
}

