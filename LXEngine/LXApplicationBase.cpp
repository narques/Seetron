//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "LXApplicationBase.h"
#include "LXWindow.h"

LXApplicationBase::LXApplicationBase(LXWindow* windowBase)
{
	_windowBase = windowBase;
}

LXApplicationBase::~LXApplicationBase()
{
}

int LXApplicationBase::run()
{
	MSG msg;
	memset(&msg, 0, sizeof(msg));

	bool Quit = false;
	while (!Quit)
	{
		while (PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE) == TRUE)
		{
			if (msg.message == WM_QUIT)
			{
				Quit = true;
				break;
			}

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		Run();
	}
	
	return (int)msg.wParam;
}