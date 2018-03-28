//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXRenderPass.h"

class LXDirectX11;
struct ID2D1DeviceContext;

class LXRenderPassUI : public LXRenderPass
{

public:
	
	LXRenderPassUI(LXRenderer* Renderer);
	virtual ~LXRenderPassUI();

	virtual void Render(LXRenderCommandList* RenderCommandList) override;
	virtual void Resize(uint Width, uint Height) override;

private:

	void DrawStats();
	void DrawConsole();

private:
	
	ID2D1SolidColorBrush* pRedBrush = nullptr;
	ID2D1SolidColorBrush* pWhiteBrush = nullptr;
	ID2D1SolidColorBrush* pYellowBrush = nullptr;
	ID2D1SolidColorBrush* pBlackBrush = nullptr;
	ID2D1SolidColorBrush* pGrayBrush = nullptr;
	ID2D1SolidColorBrush* pDarkBlue = nullptr;
	ID2D1SolidColorBrush* pConsoleBackgroundBrush = nullptr;
};

