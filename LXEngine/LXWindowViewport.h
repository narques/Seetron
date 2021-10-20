//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXWindow.h"

class LXViewport;
class LXRenderer;

class LXENGINE_API LXWindowViewport : public LXWindow
{

public:

	LXWindowViewport(void* hwndParent);
	virtual ~LXWindowViewport();

	LXViewport* GetViewport() const { return _viewport; }
	LXRenderer* GetRenderer() const { return _renderer; }

public:

	void OnClose() override;

private:

	bool OnCommand(UINT CommandID) override;
	void OnChar(uint64 CharacterCode) override;
	void OnKeyDown(uint64 KeyCode) override;
	void OnKeyUp(uint64 KeyCode) override;
	void OnLButtonDblClk(uint64 nFlags, LXPoint pntWnd) override;
	void OnLButtonDown(uint64 nFlags, LXPoint pntWnd) override;
	void OnLButtonUp(uint64 nFlags, LXPoint pntWnd) override;
	void OnMButtonDown(uint64 nFlags, LXPoint pntWnd) override;
	void OnMButtonUp(uint64 nFlags, LXPoint pntWnd) override;
	void OnMouseMove(uint64 nFlags, LXPoint pntWnd) override;
	void OnMouseWheel(uint16 nFlags, short zDelta, LXPoint pntWnd) override;
	void OnPaint() override;
	void OnRButtonDown(uint64 nFlags, LXPoint pntWnd) override;
	void OnRButtonUp(uint64 nFlags, LXPoint pntWnd) override;
	void OnSize(int width, int height) override;

private:

	LXViewport*  _viewport = nullptr;
	LXRenderer* _renderer = nullptr;

};