//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "LXString.h"

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
	int i = Find(pszSub) + (int)wcslen(pszSub);
	CHK(i != -1);
	return LXString(m_str.substr(i, m_str.size() - i).c_str());
}

LXStringA LXString::ToStringA() const
{
	int len = (int)m_str.size();
	char* sz = new char[len + 1];
	int size = (int)wcstombs(sz, m_str.c_str(), len + 1);
	sz[size] = '\0';
	LXStringA str(sz);
	delete sz;
	return str;
}

LXString LXString::Number(int i)
{
	wchar_t sz[256];
	 _itow(i, sz, 10);
	return LXString(sz);
}

LXString LXString::Number(__int64 i)
{
	return LXString(std::to_wstring(i));
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

int LXStringA::Replace(const char* pszOld, const char* pszNew)
{
	LXStringA strToReplace(pszOld);
	int nSize = strToReplace.size();
	int nPos = (int)m_str.find(strToReplace);

	while (nPos != -1)
	{
		LXStringA strReplacement(pszNew);
		m_str.replace(nPos, nSize, strReplacement);
		nPos = (int)m_str.find(strToReplace);
	}

	return 0;
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