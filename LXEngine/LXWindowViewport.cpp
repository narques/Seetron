//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "pch.h"
#include "LXWindowViewport.h"
#include "LXViewport.h"
#include "LXRenderer.h"

LXWindowViewport::LXWindowViewport(void* hWndParent)
{
	Create((HWND)hWndParent);

	// Rendering
	_viewport = new LXViewport();
	_renderer = new LXRenderer(_hWND);
	_renderer->SetViewport(_viewport);
	RECT Rect;

	GetClientRect(_hWND, &Rect);
	_viewport->SetViewportSize(Rect.right - Rect.left, Rect.bottom - Rect.top);
}

LXWindowViewport::~LXWindowViewport()
{

}

void LXWindowViewport::OnPaint()
{
	if (_viewport)
		_viewport->OnPaint();
}

void LXWindowViewport::OnSize(int width, int height)
{
	if (_viewport)
		_viewport->SetViewportSize(width, height);
}

void LXWindowViewport::OnLButtonDblClk(uint64 nFlags, LXPoint pntWnd)
{
	if (_viewport)
		_viewport->OnLButtonDblClk(nFlags, pntWnd);
}

void LXWindowViewport::OnLButtonDown(uint64 nFlags, LXPoint pntWnd)
{
	if (_viewport)
		_viewport->OnLButtonDown(nFlags, pntWnd);
}

void LXWindowViewport::OnLButtonUp(uint64 nFlags, LXPoint pntWnd)
{
	SetFocus(_hWND);
	if (_viewport)
		_viewport->OnLButtonUp(nFlags, pntWnd);
}

void LXWindowViewport::OnMButtonDown(uint64 nFlags, LXPoint pntWnd)
{
	if (_viewport)
		_viewport->OnMButtonDown(nFlags, pntWnd);
}

void LXWindowViewport::OnMButtonUp(uint64 nFlags, LXPoint pntWnd)
{
	if (_viewport)
		_viewport->OnMButtonUp(nFlags, pntWnd);
}

void LXWindowViewport::OnRButtonDown(uint64 nFlags, LXPoint pntWnd)
{
	if (_viewport)
		_viewport->OnRButtonDown(nFlags, pntWnd);
}

void LXWindowViewport::OnRButtonUp(uint64 nFlags, LXPoint pntWnd)
{
	if (_viewport)
		_viewport->OnRButtonUp(nFlags, pntWnd);
}

void LXWindowViewport::OnMouseWheel(uint16 nFlags, short zDelta, LXPoint)
{
	if (_viewport)
	{
		POINT point;
		::GetCursorPos(&point);
		::ScreenToClient(_hWND, &point);
		_viewport->OnMouseWheel(nFlags, zDelta, LXPoint(point.x, point.y));
	}
}

void LXWindowViewport::OnMouseMove(uint64 nFlags, LXPoint pntWnd)
{
	if (_viewport)
		_viewport->OnMouseMove(nFlags, pntWnd);
}

void LXWindowViewport::OnKeyUp(uint64 KeyCode)
{
	if (_viewport)
		_viewport->OnKeyUp(KeyCode, 0, 0);
}

void LXWindowViewport::OnKeyDown(uint64 KeyCode)
{
	if (_viewport)
		_viewport->OnKeyDown(KeyCode, 0, 0);
}

bool LXWindowViewport::OnCommand(UINT CommandID)
{
	return false;
}

void LXWindowViewport::OnChar(uint64 CharacterCode)
{
	if (_viewport)
		_viewport->OnChar(CharacterCode);
}

void LXWindowViewport::OnClose()
{
	LX_SAFE_DELETE(_renderer);
	LX_SAFE_DELETE(_viewport);
	LXWindow::OnClose();
}
