//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXCore/LXObject.h"
#include "LXVariant.h"
#include "LXPerformance.h"

#ifdef LX_COUNTERS

#define LX_COUNTERS_PRIMITIVE

class LXENGINE_API LXCounter : public LXObject
{
		
public:

	LXCounter(const LXString& expression);
	virtual ~LXCounter(void);
	LXString GetSentence();
	
public:	

	LXString _expression;
	double _val;
};

typedef std::vector<LXCounter*> ArrayCounters;

#define LX_COUNT(expression, value) { static LXCounter counter(expression); counter._val = value; }

#endif

class LXENGINE_API LXPerformanceLogger : protected LXPerformance
{

public:

	LXPerformanceLogger(const wchar_t* name);
	~LXPerformanceLogger();

private:

	std::wstring _name;
};

class LXENGINE_API LXPerformanceScope : protected LXPerformance
{

public:

	LXPerformanceScope(const wchar_t* name);
	~LXPerformanceScope();
	void Update();

private:
		
	std::wstring _name;

};

#define LX_CYCLEPERFOSCOPE(Name) static LXPerformanceScope PerformanceScope##Name(L#Name); \
PerformanceScope##Name.Update();
#define LX_PERFOSCOPE(Name) LXPerformanceScope PerformanceScope##Name(L#Name);




class LXCountScopeIncrement
{

public:

	LXCountScopeIncrement(const wchar_t* Name);

};

class LXCountScopeDecrement
{

public:

	LXCountScopeDecrement(const wchar_t* Name);

};


#define LX_COUNTSCOPEINC(Name) LXCountScopeIncrement CountScopeIncrement##Name(L#Name);
#define LX_COUNTSCOPEDEC(Name) LXCountScopeDecrement CountScopeDecrement##Name(L#Name);