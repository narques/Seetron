//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXPlatform.h"

class LXCORE_API LXLibrary
{

public:

	LXLibrary();
	~LXLibrary();

	bool Load(wchar_t* fileName);
	void* GetFunction(const char* procName);
	void Release();
	
private:

	void* _hModule = nullptr;
};

