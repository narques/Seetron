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
#include "LXConsoleManager.h"

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
	AddToViewDebugger(Name, Name, TextureD3D11, TextureChannel);
}

void LXRenderPipeline::AddToViewDebugger(const LXString& name, const LXString& commandName, const LXTextureD3D11* textureD3D11, ETextureChannel textureChannel)
{

	for (auto &It : _DebugTextures)
	{
		if (It.Name == name)
		{
			// Update 
			It.TextureD3D11 = textureD3D11;

			// the buffer can change, update the command name. 
			It.ConsoleCommand->Name = commandName;
			return;
		}
	}

	_DebugTextures.push_back(TVisualizableBuffer(name, textureD3D11, textureChannel));

	_DebugTextures.back().ConsoleCommand = new LXConsoleCommandNoArg(commandName, [this, name]()
	{
		// Retrieve the texture to debug.
		auto it = find_if(_DebugTextures.begin(), _DebugTextures.end(), [name](TVisualizableBuffer& visualizableBuffer)
		{
			return visualizableBuffer.Name == name;
		});
		if (it != _DebugTextures.end())
		{
			// If already set, toggle.
			if (_debugTexture == &(*it))
			{
				_debugTexture = nullptr;
			}
			else
			{
				_debugTexture = &(*it);
			}
		}
		else
		{
			CHK(0);
			_debugTexture = nullptr;
		}
	});
}


