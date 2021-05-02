//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXMutex.h"

struct LXStat
{
	std::wstring Name;
	double Time = 0.0;
	uint64 Hits = 0;
	LXStat* Parent = nullptr;
	std::set<LXStat*> Children;
};

class LXStatManager
{

public:

	LXStatManager();
	~LXStatManager();

	
	// Counters

	void UpdateCounter(const std::wstring& Name, int Value);

	const std::map<std::wstring, int>& GetCounters() const
	{
		return _Counters;
	}

	// Performances

	void Reset();
	
	void OpenStat(const std::wstring& Name);
	void UpdateAndCloseStat(const std::wstring& Name, double Value);
	
	const std::map<std::wstring, LXStat>& GetPerfomances() const
	{
		return _Stats;
	}

	const std::map<DWORD, LXStat*>& GetPerformanceByThread() const
	{
		return _StatRoots;
	}

private:


	// All stats
	std::map<std::wstring, LXStat> _Stats;

	// Stats hierarchy
	std::map<DWORD, LXStat*> _StatRoots;
	std::map<DWORD, LXStat*> _StatCurrents;

	// Counters
	LXMutex _mutex;
	std::map<std::wstring, int> _Counters;
};
