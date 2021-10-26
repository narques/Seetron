//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "pch.h"
#include "LXLibrary.h"

LXLibrary::LXLibrary()
{
}

LXLibrary::~LXLibrary()
{
}

bool LXLibrary::Load(const wchar_t* fileName)
{
	_hModule = ::LoadLibrary(fileName);
	return true;
}

void* LXLibrary::GetFunction(const char* procName)
{
	if (!_hModule)
		return nullptr;

	return ::GetProcAddress((HMODULE)_hModule, procName);
}

void LXLibrary::Release()
{
	if (_hModule)
	{
		::FreeLibrary((HMODULE)_hModule);
		_hModule = NULL;
	}
}
