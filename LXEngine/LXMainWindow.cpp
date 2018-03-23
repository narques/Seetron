//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "LXMainWindow.h"
#include "LXViewport.h"
#include "LXRenderer.h"
#include "LXCore.h"
#include "LXConsoleManager.h"
#include "resource.h"
#include "LXMemory.h" // --- Must be the last included ---
#include "LXConsoleCommands.h"

LXCoreWindow::LXCoreWindow()
{
	Create();
	Init();
}

LXCoreWindow::LXCoreWindow(HWND hWND):LXWindow(hWND)
{
	Init();
}

void LXCoreWindow::Init()
{
	CHK(_hWND);
	_Viewport = new LXViewport();
	_Renderer = new LXRenderer(_hWND);
	_Renderer->SetViewport(_Viewport);
	RECT Rect;
	GetClientRect(_hWND, &Rect);
	_Viewport->SetViewportSize(Rect.right - Rect.left, Rect.bottom - Rect.top);
}

LXCoreWindow::~LXCoreWindow()
{
}

void LXCoreWindow::OnCreate()
{
	BuildMenu();
}

void LXCoreWindow::OnClose()
{
	LX_SAFE_DELETE(_Renderer);
	LX_SAFE_DELETE(_Viewport);
	LXWindow::OnClose();
}

void LXCoreWindow::OnPaint()
{
	if (_Viewport)
		_Viewport->OnPaint();
}

void LXCoreWindow::OnSize(int width, int height)
{
	if (_Viewport)
		_Viewport->SetViewportSize(width, height);
}

void LXCoreWindow::OnLButtonDblClk(uint64 nFlags, LXPoint pntWnd)
{
	if (_Viewport)
		_Viewport->OnLButtonDblClk(nFlags, pntWnd);
}

void LXCoreWindow::OnLButtonDown(uint64 nFlags, LXPoint pntWnd)
{
	if (_Viewport)
		_Viewport->OnLButtonDown(nFlags, pntWnd);
}

void LXCoreWindow::OnLButtonUp(uint64 nFlags, LXPoint pntWnd)
{
	if (_Viewport)
		_Viewport->OnLButtonUp(nFlags, pntWnd);
}

void LXCoreWindow::OnMButtonDown(uint64 nFlags, LXPoint pntWnd)
{
	if (_Viewport)
		_Viewport->OnMButtonDown(nFlags, pntWnd);
}

void LXCoreWindow::OnMButtonUp(uint64 nFlags, LXPoint pntWnd)
{
	if (_Viewport)
		_Viewport->OnMButtonUp(nFlags, pntWnd);
}

void LXCoreWindow::OnRButtonDown(uint64 nFlags, LXPoint pntWnd)
{
	if (_Viewport)
		_Viewport->OnRButtonDown(nFlags, pntWnd);
}

void LXCoreWindow::OnRButtonUp(uint64 nFlags, LXPoint pntWnd)
{
	if (_Viewport)
		_Viewport->OnRButtonUp(nFlags, pntWnd);
}

void LXCoreWindow::OnMouseWheel(uint16 nFlags, short zDelta, LXPoint pntWnd)
{
	if (_Viewport)
		_Viewport->OnMouseWheel(nFlags, zDelta, pntWnd);
}

void LXCoreWindow::OnMouseMove(uint64 nFlags, LXPoint pntWnd)
{
	if (_Viewport)
		_Viewport->OnMouseMove(nFlags, pntWnd);
}

void LXCoreWindow::OnKeyUp(uint64 KeyCode)
{
	if (_Viewport)
		_Viewport->OnKeyUp(KeyCode, 0, 0);
}

void LXCoreWindow::OnKeyDown(uint64 KeyCode)
{
	if (_Viewport)
		_Viewport->OnKeyDown(KeyCode, 0, 0);
}

void LXCoreWindow::OnChar(uint64 CharacterCode)
{
	if (_Viewport)
		_Viewport->OnChar(CharacterCode);
}

bool LXCoreWindow::OnCommand(UINT CommandID)
{
// 	switch (CommandID)
// 	{
// 	case ID_FILE_OPEN: GetConsoleManager().TryToExecute(szFileOpen/*L"File.OpenFile"*/); break;
// 	case ID_FILE_CLOSE: GetConsoleManager().TryToExecute(L"File.Close"); break;
// 	case ID_FILE_CONSOLE: GetConsoleManager().TryToExecute(L"OpenConsole"); break;
// 	case ID_FILE_QUIT: DestroyWindow(_hWND); break;;
// 	default: return false;
// 	}

	auto It = _MenuCommands.find(CommandID);
	if (It != _MenuCommands.end())
	{
		LXConsoleCommand* ConsoleCommand = It->second;
		vector<LXString> Args;
		ConsoleCommand->Execute(Args);
	}

	return true;
}

void LXCoreWindow::BuildMenu()
{
	HMENU hMenu = ::CreateMenu();

	HMENU hMenuFile = ::CreateMenu();
	::AppendMenu(hMenu, MF_POPUP | MF_STRING, (UINT_PTR)hMenuFile, L"&File");

	HMENU hMenuView = ::CreateMenu();
	::AppendMenu(hMenu, MF_POPUP | MF_STRING, (UINT_PTR)hMenuView, L"&View");

	HMENU hMenuDebug = ::CreateMenu();
	::AppendMenu(hMenu, MF_POPUP | MF_STRING, (UINT_PTR)hMenuDebug, L"&Debug");
	
	uint i = 1000;

	for (LXConsoleCommand* ConsoleCommand : GetConsoleManager().ListCommands)
	{
		if (ConsoleCommand->Name.Find(L"File.") >= 0)
		{
			::AppendMenu(hMenuFile, MF_ENABLED | MF_STRING, i, ConsoleCommand->Name.Right(L".").GetBuffer());
		}
		else
		{
			::AppendMenu(hMenuDebug, MF_ENABLED | MF_STRING, i, ConsoleCommand->Name.GetBuffer());
		}

		_MenuCommands[i] = ConsoleCommand;
		ConsoleCommand->MenuID = i;
		i++;
	}
		
	::SetMenu(_hWND, hMenu);
}

//-------------------------------------------------------------------------

LXMainWindow::LXMainWindow()
{
}

LXMainWindow::LXMainWindow(HWND hWND) :LXCoreWindow(hWND)
{
}

LXMainWindow::~LXMainWindow()
{
}

//-------------------------------------------------------------------------

LXChildWindow::LXChildWindow(HWND hWND):LXCoreWindow(hWND)
{
}


LXChildWindow::~LXChildWindow()
{
}
