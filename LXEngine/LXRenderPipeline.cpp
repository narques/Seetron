//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "LXRenderPass.h"
#include "LXRenderPipeline.h"

LXRenderPipeline::LXRenderPipeline()
{
}

LXRenderPipeline::~LXRenderPipeline()
{
}

void LXRenderPipeline::Render(LXRenderCommandList* RenderCommandList)
{
	bool ValidRenderPasses = true;

	for (LXRenderPass* RenderPass : _RenderPasses)
	{
		if (RenderPass->IsValid())
		{
			RenderPass->Render(RenderCommandList);
			_PreviousRenderPass = RenderPass;
		}
		else
		{
			ValidRenderPasses = false;
			//LogOneTimeE(LXRenderer, L"RenderPass not valid");
#pragma  message("Enable a screen message")
			break;
		}
	}

	_PreviousRenderPass = nullptr;
}

void LXRenderPipeline::AddToViewDebugger(const LXString& Name, const LXTextureD3D11* TextureD3D11, ETextureChannel TextureChannel)
{

	for (auto &It : _DebugTextures)
	{
		if (It.Name == Name)
		{
			It.TextureD3D11 = TextureD3D11;
			return;
		}
	}

	_DebugTextures.push_back(TVisualizableBuffer(Name, TextureD3D11, TextureChannel));
}

