//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "LXDebug.h"
#include "LXMemory.h" // --- Must be the last included ---

void LXDebug::LogAssertion(const wchar_t* Expression, const wchar_t* File, int Line)
{
	LogE(LXDebug, L"Expression %s failed in %s:%i. Continue ?", Expression, File, Line);
	wchar_t Message[256];
	swprintf(Message, 255, L"Expression %s failed in %s:%i. Continue ?", Expression, File, Line);
	int Resultt = MessageBox(nullptr, Message, L"Assertion failed", MB_YESNO);
	if (Resultt == IDNO)
		exit(0);
}

void LXDebug::LogAssertion(const char* Expression, const char* File, int Line)
{
	LogE(LXDebug, "Expression %s failed in %s:%i. Continue ?", Expression, File, Line);
	char Message[256];
	sprintf(Message, "Expression %s failed in %s:%i. Continue ?", Expression, File, Line);
	int Resultt = MessageBoxA(nullptr, Message, "Assertion failed", MB_YESNO);
	if (Resultt == IDNO)
		exit(0);
}
