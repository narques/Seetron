//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

class LXCORE_API LXDebug
{

public:

	__forceinline static void Assert(const wchar_t* Expression, const wchar_t* File, int Line)
	{
		if (IsDebuggerPresent())
			DebugBreak();
		else
			LogAssertion(Expression, File, Line);
	}
	
	__forceinline static void Assert(const char* Expression, const char* File, int Line)
	{
		if (IsDebuggerPresent())
			DebugBreak();
		else
			LogAssertion(Expression, File, Line);
	}

private:

	static void LogAssertion(const wchar_t* Expression, const wchar_t* File, int Line);
	static void LogAssertion(const char* Expression, const char* File, int Line);

};
