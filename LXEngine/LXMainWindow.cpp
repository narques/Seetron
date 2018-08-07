//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "resource.h"
#include "LXMainWindow.h"
#include "LXViewport.h"
#include "LXRenderer.h"
#include "LXCore.h"
#include "LXConsoleManager.h"
#include "LXConsoleCommands.h"
#include "LXEventManager.h"
#include "LXProject.h"
#include "LXViewState.h"
#include "LXMemory.h" // --- Must be the last included ---

namespace
{
	const uint ConsoleCommandStartID = 1000;
	const uint PropertyStartID = 2000;

}

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
		
	GetEventManager()->RegisterEventFunc(EEventType::ProjectLoaded, this, [this](LXEvent* Event)
	{
		LXEventResult* EventResult = static_cast<LXEventResult*>(Event);
		if (EventResult->Success)
		{
			// Update UI
			LXViewState* viewState = GetProject()->GetActiveView();
			LXPropertyEnum* propertyEnum = dynamic_cast<LXPropertyEnum*>(viewState->GetProperty(L"ShowBuffer"));
			CHK(propertyEnum);
			
			const ArrayUint& arrayValues = propertyEnum->GetValues();
			int i = 0;
			for (const LXString& str : propertyEnum->GetChoices())
			{
				AppendMenu(_hMenuViewBuffer, str.GetBuffer(), propertyEnum, arrayValues[i++]);
			}
		}
	});

	GetEventManager()->RegisterEventFunc(EEventType::ProjectClosed, this, [this](LXEvent* Event)
	{
		int itemCount = GetMenuItemCount(_hMenuViewBuffer);
		for (int i = 0; i < itemCount; i++)
		{
			::RemoveMenu(_hMenuViewBuffer, 0, MF_BYPOSITION);
		}
	});
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
	else
	{
		auto It2 = _MenuProperties.find(CommandID);
		if (It2 != _MenuProperties.end())
		{
			LXProperty* Property = It2->second.first;
			if (LXPropertyEnum* PropertyEnum = dynamic_cast<LXPropertyEnum*>(Property))
			{
				PropertyEnum->SetValue(It2->second.second, true);
			}
		}
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

	_hMenuViewBuffer = ::CreateMenu();
	::AppendMenu(hMenuView, MF_POPUP | MF_STRING, (UINT_PTR)_hMenuViewBuffer, L"Buffer");
	
	HMENU hMenuDebug = ::CreateMenu();
	::AppendMenu(hMenu, MF_POPUP | MF_STRING, (UINT_PTR)hMenuDebug, L"&Debug");
	
	for (LXConsoleCommand* ConsoleCommand : GetConsoleManager().ListCommands)
	{
		if (ConsoleCommand->Name.Find(L"File.") >= 0)
		{
			AppendMenu(hMenuFile, ConsoleCommand->Name.Right(L".").GetBuffer(), ConsoleCommand);
		}
		if (ConsoleCommand->Name.Find(L"View.") >= 0)
		{
			AppendMenu(hMenuView, ConsoleCommand->Name.Right(L".").GetBuffer(), ConsoleCommand);
		}
		else
		{
			AppendMenu(hMenuDebug, ConsoleCommand->Name.Right(L".").GetBuffer(), ConsoleCommand);
		}
	}
		
	::SetMenu(_hWND, hMenu);
}

void LXCoreWindow::AppendMenu(HMENU hMenu, const wchar_t* Caption, LXConsoleCommand* ConsoleCommand)
{
	static uint i = ConsoleCommandStartID;
	::AppendMenu(hMenu, MF_ENABLED | MF_STRING, i, Caption);
	_MenuCommands[i] = ConsoleCommand;
	ConsoleCommand->MenuID = i;
	i++;
}

void  LXCoreWindow::AppendMenu(HMENU hMenu, const wchar_t* Caption, LXProperty* Property, uint UserData)
{
	static uint i = PropertyStartID;
	::AppendMenu(hMenu, MF_ENABLED | MF_STRING, i, Caption);
	_MenuProperties[i] = pair<LXProperty*,uint>(Property, UserData);
	i++;
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
