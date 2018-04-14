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
	for(const auto& Counter: _Counters)
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
	for (auto &Stat : _Stats)
	{
		Stat.second.Time = 0.0;
		Stat.second.Hits = 0;
	}
}

void LXStatManager::UpdateCounter(const wstring& Name, uint Value)
{
	_Counters[Name] += Value;
}

void LXStatManager::OpenStat(const wstring& Name)
{
	LXStat& Stat = _Stats[Name];
	Stat.Name = Name;
	DWORD ThreadId = ::GetCurrentThreadId();

	// Create the root if needed
	if (_StatRoots.find(ThreadId) == _StatRoots.end())
	{
		_StatRoots[ThreadId] = &Stat;
		_StatCurrents[ThreadId] = nullptr;
	}
		
	LXStat* StatCurrent = _StatCurrents[ThreadId];
	if (StatCurrent)
	{
		Stat.Parent = StatCurrent;
		StatCurrent->Children.insert(&Stat);
	}
	_StatCurrents[ThreadId] = &Stat;
	
}

void LXStatManager::UpdateAndCloseStat(const wstring& Name, double Value)
{
	LXStat& Stat = _Stats[Name];
	Stat.Time+= Value;
	Stat.Hits++;
	DWORD ThreadId = ::GetCurrentThreadId();
	_StatCurrents[ThreadId] = Stat.Parent;
}
