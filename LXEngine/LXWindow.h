//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXPlatform.h"
#include "LXPoint.h"

class LXENGINE_API LXWindow
{

public:

	LXWindow();
	LXWindow(HWND hWND); //Just Attach to an existing window
	virtual ~LXWindow();

	virtual bool OnCommand(UINT CommandID) { return false; };
	virtual void OnChar(uint64 CharacterCode) {};
	virtual void OnClose();
	virtual void OnCreate();
	virtual void OnDestroy();
	virtual void OnKeyDown(uint64 KeyCode) {};
	virtual void OnKeyUp(uint64 KeyCode) {};
	virtual void OnLButtonDblClk(uint64 nFlags, LXPoint pntWnd) {};
	virtual void OnLButtonDown(uint64 nFlags, LXPoint pntWnd) {};
	virtual void OnLButtonUp(uint64 nFlags, LXPoint pntWnd) {};
	virtual void OnMButtonDown(uint64 nFlags, LXPoint pntWnd) {};
	virtual void OnMButtonUp(uint64 nFlags, LXPoint pntWnd) {};
	virtual void OnMouseMove(uint64 nFlags, LXPoint pntWnd) {};
	virtual void OnMouseWheel(uint16 nFlags, short zDelta, LXPoint pntWnd) {};
	virtual void OnRButtonDown(uint64 nFlags, LXPoint pntWnd) {};
	virtual void OnRButtonUp(uint64 nFlags, LXPoint pntWnd) {};
	virtual void OnSize(int width, int height) {};

	virtual void OnPaint() {}; // NO VM_PAINT on LXWindow...
	
	void SetHandle(HWND hWND) { _hWND = hWND; }
	HWND GetHandle() const { return _hWND; }
	LRESULT Run(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);

protected:

	bool Create(HWND hWndParent);

private:
	
		
protected:

	bool _bAttachedToExistingHWND = false;
	HWND _hWND = NULL;

};

