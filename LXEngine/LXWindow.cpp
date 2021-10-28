//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "pch.h"
#include "LXWindow.h"
#include <strsafe.h>
#include <winuser.h>
//#include "resource.h"
#include "LXConsoleManager.h"
#include "LXConsoleCommands.h"

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
	wcex.hIcon = 0;// LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
#pragma message("Remove IDR_MAINMENU form resource")
	wcex.lpszMenuName =  0;// MAKEINTRESOURCEW(IDR_MAINMENU);
	wcex.lpszClassName = szWindowClassName;
	wcex.hIconSm = nullptr;

	return RegisterClassEx(&wcex);
}

LXWindow::LXWindow()
{
	
}

LXWindow::LXWindow(HWND hWnd)
{
	_bAttachedToExistingHWND = true;
	_hWND = hWnd;
}

LXWindow::~LXWindow()
{
}

void LXWindow::OnClose()
{
	if (!_bAttachedToExistingHWND)
	{
		BOOL res = ::DestroyWindow(_hWND);
		CHK(res);
	}
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

bool LXWindow::Create(HWND hWndParent)
{
#ifdef LX_ENGINE_STATIC
	HINSTANCE hInstance = GetModuleHandle(nullptr);
#else
	HINSTANCE hInstance = GetModuleHandle(L"LXEngine.dll");
#endif
	CHK(hInstance);
	registerClass(hInstance);
	
	DWORD style = 0;
	
	if (hWndParent)
	{
		style = WS_CHILD | WS_VISIBLE;
	}
	else
	{
		style = WS_OVERLAPPEDWINDOW;
	}
	
	_hWND = ::CreateWindow(szWindowClassName, szWindowTitle, style, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, hWndParent, nullptr, hInstance, this);

	if (_hWND)
	{
		if (hWndParent)
		{
			style = GetWindowLong(_hWND, GWL_STYLE);
			style = style & ~(WS_CAPTION | WS_THICKFRAME);
			SetWindowLong(_hWND, GWL_STYLE, style);
		}
		else
		{
			SetWindowText(_hWND, szWindowTitle);
		}

		ShowWindow(_hWND, 1);
		UpdateWindow(_hWND);
		return true;
	}
	else
	{
		return false;
	}
}

void LXWindow::OnCreate()
{
}

void LXWindow::OnDestroy()
{
	PostQuitMessage(0);
}
