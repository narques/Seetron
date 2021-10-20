//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "LXWindow.h"

class LXViewport;
class LXRenderer;
class LXConsoleCommand;
class LXProperty;

class LXENGINE_API LXCoreWindow : public LXWindow 
{

public:

	LXCoreWindow();
	LXCoreWindow(HWND hWND);
	virtual ~LXCoreWindow();

	LXViewport*	GetViewport() const { return _Viewport; }
	LXRenderer* GetRenderer() const { return _Renderer; }

	bool OnCommand(UINT CommandID) override;
	void OnChar(uint64 CharacterCode) override;
	void OnCreate() override;
	void OnClose() override;
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

	void Init();
	void BuildMenu();
	void AppendMenu(HMENU hMenu, const wchar_t* Caption, LXConsoleCommand* ConsoleCommand);
	void AppendMenu(HMENU hMenu, const wchar_t* Caption, LXProperty* Property, uint UserData);

protected:

	LXRenderer* _Renderer = nullptr;
	LXViewport* _Viewport = nullptr;

private:
		
	HMENU _hMenuViewBuffer = 0;
	std::map<uint, LXConsoleCommand*> _MenuCommands;
	std::map<uint, std::pair<LXProperty*, uint>> _MenuProperties;
};

class LXENGINE_API LXMainWindow : public LXCoreWindow
{

public:

	LXMainWindow();
	LXMainWindow(HWND hWND);
	virtual ~LXMainWindow();

};

class LXENGINE_API LXChildWindow : public LXCoreWindow
{

public:

	LXChildWindow(HWND hWND);
	virtual ~LXChildWindow();

};

