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
	double Time = 0.0;
	uint64 Hits = 0;
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
		return Counters;
	}

	// Performances

	void Reset();
	
	void UpdateStat(const wstring& Name, double Value);
	
	const map<wstring, LXStat>& GetPerfomances() const
	{
		return Stats;
	}

private:

	map<wstring, LXStat> Stats;
	map<wstring, int> Counters;
};
