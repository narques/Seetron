//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXObject.h"
#include "LXVariant.h"
#include "LXPerformance.h"

#ifdef LX_COUNTERS

#define LX_COUNTERS_PRIMITIVE

class LXCORE_API LXCounter : public LXObject
{
		
public:

	LXCounter(const LXString& expression);
	virtual ~LXCounter(void);
	LXString GetSentence();
	
public:	

	LXString _expression;
	double _val;
};

typedef vector<LXCounter*> ArrayCounters;

#define LX_COUNT(expression, value) { static LXCounter counter(expression); counter._val = value; }

#endif


class LXPerformanceScope : protected LXPerformance
{

public:

	LXPerformanceScope(const wchar_t* Name);
	~LXPerformanceScope();

private:
		
	wstring _Name;

};

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