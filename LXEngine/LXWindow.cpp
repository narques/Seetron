//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "LXWindow.h"
#include <strsafe.h>
#include <winuser.h>
#include "resource.h"
#include "LXConsoleManager.h"
#include "LXConsoleCommands.h"
#include "LXMemory.h" // --- Must be the last included ---

LXWindow* pWindowBase = nullptr;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_NCCREATE:
		{
			LPCREATESTRUCT cs = (LPCREATESTRUCT)lParam;
			pWindowBase = (LXWindow*)cs->lpCreateParams; // Window Class
			pWindowBase->SetHandle(hWnd);
			return TRUE;
		}
		break;

		default:
		{
			if (pWindowBase)
			{
				return pWindowBase->Run(hWnd, message, wParam, lParam);
			}
			else
			{
				return DefWindowProc(hWnd, message, wParam, lParam);
			}
		}
	}
}

const wchar_t szWindowClassName[] = L"LXWINDOW";
const wchar_t szWindowTitle[] = L"Seetron Engine - Demo Player";

ATOM registerClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
#pragma message("Remove IDR_MAINMENU form resource")
	wcex.lpszMenuName =  0;// MAKEINTRESOURCEW(IDR_MAINMENU);
	wcex.lpszClassName = szWindowClassName;
	wcex.hIconSm = nullptr;

	return RegisterClassEx(&wcex);
}

HWND createWindow(void* lpParam)
{
#ifdef LX_CORE_STATIC
	HINSTANCE hInstance = GetModuleHandle(nullptr);
#else
	HINSTANCE hInstance = GetModuleHandle(L"LXCore.dll");
#endif
	CHK(hInstance);
	registerClass(hInstance);

	HWND hWnd = ::CreateWindow(szWindowClassName, szWindowTitle, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, lpParam);

	SetWindowText(hWnd, szWindowTitle);
	ShowWindow(hWnd, 1);
	UpdateWindow(hWnd);
	return hWnd;
}

LXWindow::LXWindow()
{
	
}

LXWindow::LXWindow(HWND hWND)
{
	_hWND = hWND;
}

LXWindow::~LXWindow()
{
}

void LXWindow::OnClose()
{
	::DestroyWindow(_hWND);
}

LRESULT LXWindow::Run(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	switch (Message)
	{
		case WM_CREATE: pWindowBase->OnCreate(); break;
		case WM_SIZE: pWindowBase->OnSize(LOWORD(lParam), HIWORD(lParam)); break;
		case WM_MOUSEMOVE: pWindowBase->OnMouseMove(wParam, LXPoint(LOWORD(lParam), HIWORD(lParam))); break;
		case WM_MOUSEWHEEL: pWindowBase->OnMouseWheel(GET_KEYSTATE_WPARAM(wParam), GET_WHEEL_DELTA_WPARAM(wParam), LXPoint(LOWORD(lParam), HIWORD(lParam))); break;
		case WM_LBUTTONDOWN: pWindowBase->OnLButtonDown(wParam, LXPoint(LOWORD(lParam), HIWORD(lParam))); break;
		case WM_LBUTTONUP: pWindowBase->OnLButtonUp(wParam, LXPoint(LOWORD(lParam), HIWORD(lParam))); break;
		case WM_MBUTTONDOWN: pWindowBase->OnMButtonDown(wParam, LXPoint(LOWORD(lParam), HIWORD(lParam))); break;
		case WM_MBUTTONUP: pWindowBase->OnMButtonUp(wParam, LXPoint(LOWORD(lParam), HIWORD(lParam))); break;
		case WM_RBUTTONDOWN: pWindowBase->OnRButtonDown(wParam, LXPoint(LOWORD(lParam), HIWORD(lParam))); break;
		case WM_RBUTTONUP: pWindowBase->OnRButtonUp(wParam, LXPoint(LOWORD(lParam), HIWORD(lParam))); break;
		case WM_KEYUP: pWindowBase->OnKeyUp(wParam); break;
		case WM_KEYDOWN: pWindowBase->OnKeyDown(wParam); break;
		case WM_CHAR: pWindowBase->OnChar(wParam); break;
		case WM_CLOSE: pWindowBase->OnClose(); break;
		case WM_DESTROY: pWindowBase->OnDestroy(); break;
		case WM_LBUTTONDBLCLK: pWindowBase->OnLButtonDblClk(wParam, LXPoint(LOWORD(lParam), HIWORD(lParam))); break;
		case WM_COMMAND: 
		{
			if (!pWindowBase->OnCommand(LOWORD(wParam)))
			{
				return DefWindowProc(hWnd, Message, wParam, lParam);
			}
		}
		break;

		default:
		{
			return DefWindowProc(hWnd, Message, wParam, lParam);
		}
	}

	return 0;
}

bool LXWindow::Create()
{
	_hWND = createWindow(this);
	return true;
}

void LXWindow::OnCreate()
{
}

void LXWindow::OnDestroy()
{
	PostQuitMessage(0);
}
