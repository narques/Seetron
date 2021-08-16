//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "LXRenderPassUI.h"
#include "LXConsoleManager.h"
#include "LXCore.h"
#include "LXDirectX11.h"
#include "LXRenderer.h"
#include "LXRenderPipeline.h"
#include "LXStatManager.h"
#include "LXStatistic.h"
#include "LXViewport.h"

//------------------------------------------------------------------------------------------------------
// Console commands and Settings
//------------------------------------------------------------------------------------------------------

namespace
{
	const float textShadowOffset = 2.f;
}

bool ShowFPS = false;
LXConsoleCommandT<bool> CCShowFPS(L"View.ShowFPS", &ShowFPS);

LXRenderPassUI::LXRenderPassUI(LXRenderer* InRenderer):LXRenderPass(InRenderer)
{
	const LXDirectX11* DirectX11 = Renderer->GetDirectX11();
#if LX_D2D1
	DirectX11->_D2D1DeviceContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Red), &pRedBrush);
	DirectX11->_D2D1DeviceContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::GreenYellow), &greenBrush);
	DirectX11->_D2D1DeviceContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &pWhiteBrush);
	DirectX11->_D2D1DeviceContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &pBlackBrush);
	DirectX11->_D2D1DeviceContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::LightGray), &pGrayBrush);
	DirectX11->_D2D1DeviceContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Yellow), &pYellowBrush);
	DirectX11->_D2D1DeviceContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::DarkBlue), &pDarkBlue);
	DirectX11->_D2D1DeviceContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &pConsoleBackgroundBrush);

	statForegroundBrush = greenBrush;
#endif
}

LXRenderPassUI::~LXRenderPassUI()
{
	LX_SAFE_RELEASE(pRedBrush);
	LX_SAFE_RELEASE(greenBrush);
	LX_SAFE_RELEASE(pWhiteBrush);
	LX_SAFE_RELEASE(pYellowBrush);
	LX_SAFE_RELEASE(pBlackBrush);
	LX_SAFE_RELEASE(pGrayBrush);
	LX_SAFE_RELEASE(pDarkBlue);
	LX_SAFE_RELEASE(pConsoleBackgroundBrush);
}

void LXRenderPassUI::Resize(uint Width, uint Height)
{

}

void LXRenderPassUI::DrawStats(const LXStat* Stat, float X, float& Y)
{
#if LX_D2D1
	const LXDirectX11* DirectX11 = Renderer->GetDirectX11();
	ID2D1DeviceContext* dc = DirectX11->_D2D1DeviceContext;
	float Bottom = Y + LXPlatform::GetFontSize();
	LXString Str = LXString::Format(L"%s : %.2f MS : %i Hit(s)\n",
		Stat->Name.c_str(),
		Stat->Time,
		Stat->Hits);
	dc->DrawText(Str.GetBuffer(), Str.GetLength(), DirectX11->_DWriteTextFormat, D2D1::RectF(X + textShadowOffset, Y + textShadowOffset, 600, Bottom), pBlackBrush);
	dc->DrawText(Str.GetBuffer(), Str.GetLength(), DirectX11->_DWriteTextFormat, D2D1::RectF(X, Y, 600, Bottom), statForegroundBrush);
	Y += LXPlatform::GetFontSize();

	for (const LXStat* Child : Stat->Children)
	{
		DrawStats(Child, X + 10, Y);
	}
#endif
}

void LXRenderPassUI::DrawStats()
{

#if LX_D2D1

	if (!ShowFPS)
	{
		GetStatManager()->Reset();
		return;
	}

	const LXDirectX11* DirectX11 = Renderer->GetDirectX11();
	ID2D1DeviceContext* dc = DirectX11->_D2D1DeviceContext;

	const int LineHeight = LXPlatform::GetFontSize();

	float PositionY = 0.f;
	ArrayCounters& arrayCounters = GetCore().GetCounters();
	for (LXCounter* Counter : arrayCounters)
	{
		float Top = (float)PositionY;
		float Bottom = Top + (float)LineHeight;
		LXString Str = Counter->GetSentence();
		dc->DrawText(Str.GetBuffer(), Str.GetLength(), DirectX11->_DWriteTextFormat, D2D1::RectF(textShadowOffset, Top + textShadowOffset, 600, Bottom), pBlackBrush);
		dc->DrawText(Str.GetBuffer(), Str.GetLength(), DirectX11->_DWriteTextFormat, D2D1::RectF(0.0f, Top, 600, Bottom), statForegroundBrush);
		PositionY += LineHeight;
	}

	// A Line space
	PositionY += LineHeight;

	// Performance Counters
	for (auto &StatsByThread : GetStatManager()->GetPerformanceByThread())
	{
		DrawStats(StatsByThread.second, 0.f, PositionY);
		PositionY += LXPlatform::GetFontSize();
	}

	// A Line space
	PositionY += LineHeight;

	// Counters
	for (auto& Counter : GetStatManager()->GetCounters())
	{
		float Top = (float)PositionY;
		float Bottom = Top + (float)LineHeight;
		LXString Str = LXString::Format(L"%s : %i\n",
			Counter.first.c_str(),
			Counter.second);

		DirectX11->_D2D1DeviceContext->DrawText(Str.GetBuffer(), Str.GetLength(), DirectX11->_DWriteTextFormat, D2D1::RectF(textShadowOffset, Top + textShadowOffset, 600, Bottom), pBlackBrush);
		DirectX11->_D2D1DeviceContext->DrawText(Str.GetBuffer(), Str.GetLength(), DirectX11->_DWriteTextFormat, D2D1::RectF(0.0f, Top, 600, Bottom), statForegroundBrush);
		PositionY += LineHeight;
	}

#endif

	GetStatManager()->Reset();

}

void LXRenderPassUI::DrawConsole()
{

#if LX_D2D1

	if (!Renderer->GetViewport()->ConsoleEnabled)
		return;

	const LXDirectX11* DirectX11 = Renderer->GetDirectX11();
	ID2D1DeviceContext* dc = DirectX11->_D2D1DeviceContext;
	
	const LXViewport* Viewport = Renderer->GetViewport();
		
	const float LineHeight = LXPlatform::GetFontSize();

	// Background
	auto RectBackground = D2D1::RectF(5.0f, Viewport->GetHeight() - (CONSOLE_MAX_LINE * LineHeight) - 5.0f, Viewport->GetWidth() - 5.0f, Viewport->GetHeight() - 5.0f);
	DirectX11->_D2D1DeviceContext->FillRectangle(RectBackground, pConsoleBackgroundBrush);

	// Previous lines
	int i = 0;
	for (const LXString& Message : Renderer->GetConsoleBuffer())
	{
		float Top = Viewport->GetHeight() - ((Renderer->GetConsoleBuffer().size() - i + 1) * LineHeight) - 5.0f;
		float Bottom = Top + LineHeight;
		auto Rect = D2D1::RectF(5.0f, Top, Viewport->GetWidth() - 5.0f, Bottom);
		DirectX11->_D2D1DeviceContext->DrawTextW(Message, Message.size(), DirectX11->_DWriteTextFormat, Rect, statForegroundBrush);
		i++;
	}

		// Input line
	auto RectTextLine = D2D1::RectF(5.0f, Viewport->GetHeight() - LineHeight - 5.0f, Viewport->GetWidth() - 5.0f, Viewport->GetHeight() - 5.0f);
	LXString Input = L">";
	if (Viewport->ConsoleNearestCommand.size())
		Input += Viewport->ConsoleNearestCommand[Viewport->ConsoleSuggestionSeek];
	DirectX11->_D2D1DeviceContext->DrawTextW(Input, Input.size(), DirectX11->_DWriteTextFormat, RectTextLine, pGrayBrush);
	Input = L">" + Viewport->ConsoleString + "_";
	DirectX11->_D2D1DeviceContext->DrawTextW(Input, Input.size(), DirectX11->_DWriteTextFormat, RectTextLine, pYellowBrush);
#endif
}

void LXRenderPassUI::Render(LXRenderCommandList*)
{
	const LXRenderPass* RenderPass = Renderer->GetRenderPipeline()->GetPreviousRenderPass();
	const LXDirectX11* DirectX11 = Renderer->GetDirectX11();

#if LX_D2D1
	ID2D1DeviceContext* dc = DirectX11->_D2D1DeviceContext;
#endif
	
	DirectX11->_D3DUserDefinedAnnotation->BeginEvent(L"UI");

#if LX_D2D1
	dc->BeginDraw();
#endif
	
	DrawStats();
	DrawConsole();

#if LX_D2D1
	dc->EndDraw();
#endif

	DirectX11->_D3DUserDefinedAnnotation->EndEvent();
}

