//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "StdAfx.h"
#include "LXString.h"
#include <stdarg.h>
#include "LXMemory.h" // --- Must be the last included ---

LXStringA LXStringA::Number(int i)
{
	char sz[256];
	sprintf_s( sz, 256, "%i", i);
	return LXStringA(sz);
}

LXStringA LXStringA::Number(unsigned int i)
{
	return LXStringA(std::to_string(i));
}

LXStringA LXStringA::Number(unsigned __int64 u)
{
	char sz[256];
	sprintf_s( sz, 256, "%u", (int)u);
	return LXStringA(sz);
}

LXStringA LXStringA::NumberToHexa(unsigned __int64 u)
{
	char sz[256];
	sprintf_s( sz, 256, "%X", (int)u);
	return LXStringA(sz);
}

LXString LXString::Left(const wchar_t* pszSub) const
{
	int i = Find(pszSub);
	CHK(i != -1);
	return Left(i);
}

LXString LXString::Right(const wchar_t* pszSub) const
{
	int i = Find(pszSub) + wcslen(pszSub);
	CHK(i != -1);
	return LXString(m_str.substr(i, m_str.size() - i).c_str());
}

LXString LXString::Number(int i)
{
	wchar_t sz[256];
	 _itow(i, sz, 10);
	return LXString(sz);
}

LXString LXString::Number(unsigned __int64 u)
{
	return LXString(std::to_wstring(u));
}

LXString LXString::Number(double d)
{
	return LXString(std::to_wstring(d));
}

LXString LXString::NumberToHexa(unsigned __int64 u)
{
	wchar_t sz[256];
	_ui64tow(u, sz, 16);
	return LXString(sz);
}

LXString LXString::Format(const wchar_t* Format, ...)
{
	va_list arguments;
	va_start(arguments, Format);
	wchar_t out[256];
	vswprintf(out, 255, Format, arguments);
	va_end(arguments);
	return LXString(out);
}

LXStringA LXStringA::Format(const char* Format, ...)
{
	va_list arguments;
	va_start(arguments, Format);
	char out[256];
	vsprintf(out, Format, arguments);
	va_end(arguments);
	return LXStringA(out);
}

float LXString::ToFloat()
{
	return (float)_wtof(GetBuffer());
}

LXStringA LXStringA::Number(double d)
{
	return LXStringA(std::to_string(d));
}

LXString GetTab(int Count)
{
	LXString Tabs;
	for (int i = 0; i < Count; i++)
	{
		Tabs += "\t";
	}
	return Tabs;
}