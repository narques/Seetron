//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

struct LXStat
{
	wstring Name;
	double Time = 0.0;
	uint64 Hits = 0;
	LXStat* Parent = nullptr;
	set<LXStat*> Children;
};

class LXStatManager
{

public:

	LXStatManager();
	~LXStatManager();

	
	// Counters

	void UpdateCounter(const wstring& Name, uint Value);

	const map<wstring, int>& GetCounters() const
	{
		return _Counters;
	}

	// Performances

	void Reset();
	
	void OpenStat(const wstring& Name);
	void UpdateAndCloseStat(const wstring& Name, double Value);
	
	const map<wstring, LXStat>& GetPerfomances() const
	{
		return _Stats;
	}

	const map<DWORD, LXStat*>& GetPerformanceByThread() const
	{
		return _StatRoots;
	}

private:


	// All stats
	map<wstring, LXStat> _Stats;

	// Stats hierarchy
	map<DWORD, LXStat*> _StatRoots;
	map<DWORD, LXStat*> _StatCurrents;

	map<wstring, int> _Counters;
};
