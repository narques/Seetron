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
