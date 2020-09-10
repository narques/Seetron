//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXFormats.h"
#include "LXConsoleManager.h"

class LXRenderCommandList;
class LXRenderPass;
class LXTextureD3D11;

struct TVisualizableBuffer
{
	TVisualizableBuffer(const LXString& InName, const LXTextureD3D11* InTextureD3D11, ETextureChannel InTextureChannel):
		Name(InName), TextureD3D11(InTextureD3D11), TextureChannel(InTextureChannel)
	{
	}

	LXString Name;
	const LXTextureD3D11* TextureD3D11;
	ETextureChannel TextureChannel;
	LXConsoleCommandNoArg* ConsoleCommand;
};

typedef vector<TVisualizableBuffer> TDebuggableTextures;

class LXRenderPipeline 
{

public:

	LXRenderPipeline();
	virtual ~LXRenderPipeline();
	virtual void Clear() {};
	virtual void RebuildShaders() = 0;
	virtual void Resize(uint Width, uint Height) = 0;
	virtual void Render(LXRenderCommandList* RenderCommandList);
	virtual void PostRender() {};
	virtual const LXRenderPass* GetPreviousRenderPass() const = 0;
	virtual const LXTextureD3D11* GetOutput() const = 0;
	
	const TDebuggableTextures& GetDebugTextures() const { return _DebugTextures; }
	const TVisualizableBuffer* GetDebugTexture() const { return _debugTexture; }
	void AddToViewDebugger(const LXString& Name, const LXTextureD3D11* TextureD3D11, ETextureChannel TextureChannel);
	void AddToViewDebugger(const LXString& Name, const LXString& commandName, const LXTextureD3D11* TextureD3D11, ETextureChannel TextureChannel);

protected:

	vector<LXRenderPass*> _RenderPasses;
	LXRenderPass* _PreviousRenderPass = nullptr;
	TDebuggableTextures _DebugTextures;
	const TVisualizableBuffer* _debugTexture = nullptr;
};

