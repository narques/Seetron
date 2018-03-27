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

//class LXTextureD3D11;
class LXDepthStencilViewD3D11;

class LXRenderPassAux : public LXRenderPass
{

public:

	LXRenderPassAux(LXRenderer* InRenderer);
	virtual ~LXRenderPassAux();
	void Render(LXRenderCommandList* RenderCommandList) override;
	void Resize(uint Width, uint Height) override;

public:

	// Ref.
	LXViewport* Viewport;
};

