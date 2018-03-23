//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "LXPlatform.h"
#include "LXWindow.h"
#include "targetver.h"
#include <windows.h>
#include <string>
#include <Rpc.h>
#include "LXMemory.h" // --- Must be the last included ---

#pragma comment(lib, "Rpcrt4.lib") // For CreateUuid()

LXPlatform::LXPlatform()
{
}

std::wstring LXPlatform::GetApplicationFileName()
{
	wchar_t fileName[MAX_PATH];
	::GetModuleFileName(NULL, fileName, MAX_PATH);
	return fileName;
}

void LXPlatform::PostMsg(LXWindow* window, unsigned int msg)
{
	if (!window)
		return;

	HWND hWnd = (HWND)window->GetHandle();
	::PostMessage(hWnd, msg, 0, 0);
}

void LXPlatform::PostMsg(LXWindow* window, unsigned int msg, void* wParam)
{
	HWND hWnd = (HWND)window->GetHandle();
	::PostMessage(hWnd, msg, (WPARAM)wParam, 0);
}

void LXPlatform::SendMsg(LXWindow* window, unsigned int msg, void* wParam)
{
	HWND hWnd = (HWND)window->GetHandle();
	::SendMessage(hWnd, msg, (WPARAM)wParam, 0);
}

std::wstring LXPlatform::GetCurrentDate()
{
	SYSTEMTIME st;
	ZeroMemory(&st, sizeof(st));
	GetLocalTime(&st); 
	wchar_t date[128];
	swprintf(date, 127, L"%d/%02d/%02d", st.wYear, st.wMonth, st.wDay);
	return date;
}

std::wstring LXPlatform::GetCurTime()
{
	SYSTEMTIME st;
	ZeroMemory(&st, sizeof(st));
	GetLocalTime(&st);
	wchar_t time[128];
	swprintf(time, 127, L"%02d:%02d:%02d", st.wHour, st.wMinute, st.wSecond);
	return time;
}

std::wstring LXPlatform::CreateUuid()
{
	UUID uuid;
	::ZeroMemory(&uuid, sizeof(UUID));
	::UuidCreate(&uuid);
	wchar_t* str = NULL;
	UuidToString(&uuid, (RPC_WSTR*)&str);
	std::wstring strRet = str;
	if (str)
		::RpcStringFree((RPC_WSTR*)&str);
	return strRet;
}

bool LXPlatform::IsDebuggerPresent()
{
	return ::IsDebuggerPresent() == TRUE;
}

bool LXPlatform::DeleteFile(const wchar_t* Filename)
{
	return ::DeleteFile(Filename) == TRUE;
}