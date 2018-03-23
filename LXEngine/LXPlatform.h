//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXObject.h"

class LXWindow;

class LXCORE_API LXPlatform 
{

public:

	LXPlatform(void);

	static std::wstring GetApplicationFileName();
	static void PostMsg(LXWindow* window, unsigned int msg);
	static void PostMsg(LXWindow* window, unsigned int msg, void* wParam);
	static void SendMsg(LXWindow* window, unsigned int msg, void* wParam);
	static std::wstring GetCurrentDate();
	static std::wstring GetCurTime();
	static std::wstring CreateUuid();
	static bool IsDebuggerPresent();
	static bool DeleteFile(const wchar_t* Filename);
};
