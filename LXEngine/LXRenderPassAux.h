//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXRenderPass.h"
#include "LXTextureD3D11.h"

class LXRenderTarget;
class LXRenderTargetDepth;
class LXDepthStencilViewD3D11;

class LXRenderPassAux : public LXRenderPass
{

public:

	LXRenderPassAux(LXRenderer* InRenderer);
	virtual ~LXRenderPassAux();
	void Render(LXRenderCommandList* RenderCommandList) override;
	void Resize(uint Width, uint Height) override;

	const LXRenderTargetDepth* GetDepthRenderTarget() const { return _Depth; }
	const LXRenderTarget* GetColorRenderTarget() const { return _Color; }

private:

	void CreateBuffers(uint Width, uint Height);
	void DeleteBuffers();

public:

	// Ref.
	LXViewport* Viewport;

private:

	LXRenderTargetDepth* _Depth = nullptr;
	LXRenderTarget* _Color = nullptr;
};

