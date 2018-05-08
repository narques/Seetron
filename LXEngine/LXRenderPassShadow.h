//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXRenderPass.h"
#include "LXConstantBufferD3D11.h"

class LXRenderPassShadow : 	public LXRenderPass
{

public:

	LXRenderPassShadow(LXRenderer* InRenderer);
	virtual ~LXRenderPassShadow();
	void Render(LXRenderCommandList* RCL) override;
	void Resize(uint Width, uint Height) override;
	
	unique_ptr<LXTextureD3D11> TextureDepth;
	unique_ptr<LXDepthStencilViewD3D11> DepthStencilView;

	// SpotLight
	unique_ptr<LXConstantBufferD3D11> ConstantBufferSpotLight;
};

