//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "LXConsoleCommands.h"
#include "LXConsoleManager.h"
#include "LXCore.h"
#include "LXRenderer.h"
#include <commdlg.h>
#include "LXMemory.h" // --- Must be the last included ---

LXConsoleCommands::LXConsoleCommands()
{
}

LXConsoleCommands::~LXConsoleCommands()
{
}

LXConsoleCommandNoArg CCGetOpenFileName(FileOpenCommandName, []()
{
	wchar_t FileName[MAX_PATH];
	OPENFILENAME ofn = {};
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.lpstrFile = FileName;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = MAX_PATH;
	ofn.hwndOwner = ::GetActiveWindow();
	if (GetOpenFileName(&ofn))
	{
		GetCore().LoadFile(FileName);
	}
});

LXConsoleCommandNoArg CCClose(FileCloseCommandName, []()
{
	GetCore().CloseProject();
},[]()
{
	return GetCore().GetProject() != nullptr;
});

LXConsoleCommandNoArg CCQuit(FileQuitCommandName, []()
{
	HWND hWnd = ::GetActiveWindow();
	SendMessage(hWnd, WM_CLOSE, 0, 0);
});

LXConsoleCommandNoArg CCOpenConsole(L"OpenConsole", []()
{
	GetLogger().CreateOSConsole();
});

LXConsoleCommandNoArg CCShowBounds(L"View.ShowBounds", []()
{
	bool Bounds = !GetCore().GetRenderer()->ShowBounds();
	GetCore().GetRenderer()->ShowBounds(Bounds);
}, []()
{
	return GetCore().GetProject() != nullptr;
});

LXConsoleCommandNoArg CCTraceObject(L"Debug.DumpObject", []()
{
	LXObject::TraceAll();
});

