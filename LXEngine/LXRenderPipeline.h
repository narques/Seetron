//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

class LXRenderCommandList;
class LXRenderPass;
class LXTextureD3D11;

class LXRenderPipeline 
{

public:

	LXRenderPipeline();
	virtual ~LXRenderPipeline();
	virtual void RebuildShaders() = 0;
	virtual void Resize(uint Width, uint Height) = 0;
	virtual void Render(LXRenderCommandList* RenderCommandList);
	virtual void PostRender() {};
	virtual const LXRenderPass* GetPreviousRenderPass() const = 0;
	virtual const LXTextureD3D11* GetOutput() const = 0;

protected:

	vector<LXRenderPass*> _RenderPasses;
	LXRenderPass* _PreviousRenderPass = nullptr;
};

