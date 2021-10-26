//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "pch.h"
#include "LXConsoleCommands.h"
#include "LXConsoleManager.h"
#include "LXEngine.h"
#include "LXLogger.h"
#include "LXViewport.h"

#include <commdlg.h>

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
		GetEngine().LoadFile(FileName);
	}
});

LXConsoleCommandNoArg CCClose(FileCloseCommandName, []()
{
	GetEngine().CloseProject();
},[]()
{
	return GetEngine().GetProject() != nullptr;
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
	GetEngine().GetViewport()->ToggleShowBBoxes();
}, []()
{
	return GetEngine().GetProject() != nullptr;
});

LXConsoleCommandNoArg CCShowPrimtitiveBounds(L"View.ShowPrimitiveBounds", []()
	{
		GetEngine().GetViewport()->ToggleShowPrimitiveBBoxes();
	}, []()
	{
		return GetEngine().GetProject() != nullptr;
	});

LXConsoleCommandNoArg CCTraceObject(L"Debug.DumpObject", []()
{
	LXObject::TraceAll();
});

