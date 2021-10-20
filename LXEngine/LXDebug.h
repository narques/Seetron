//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXExports.h"

class LXENGINE_API LXDebug
{

public:

	static void LogFailure(const wchar_t* Expression, const wchar_t* File, int Line);
	static void LogFailure(const char* Expression, const char* File, int Line);
};