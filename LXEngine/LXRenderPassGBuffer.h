//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXRenderPass.h"

class LXRenderPassShadow;

class LXRenderPassGBuffer : public LXRenderPass
{

public:

	LXRenderPassGBuffer(LXRenderer* InRenderer);
	virtual ~LXRenderPassGBuffer();
	void Render(LXRenderCommandList* RenderCommandList) override;
	void Resize(uint Width, uint Height) override;
	
private:

	void CreateBuffers(uint Width, uint Height);
	void DeleteBuffers();
	
public:
	
	// Ref.
	LXViewport* Viewport;
	LXRenderPassShadow* RenderPassShadow;

	// D3D11 Objects
	LXTextureD3D11* TextureDepth = nullptr;
	LXTextureD3D11* TextureColor = nullptr;
	LXTextureD3D11* TextureNormal = nullptr;
	LXTextureD3D11* TextureSpecular = nullptr;
	LXTextureD3D11* TextureEmissive = nullptr;
	
	LXDepthStencilViewD3D11* DepthStencilView = nullptr;
	LXRenderTargetViewD3D11* RenderTargetColor = nullptr;
	LXRenderTargetViewD3D11* RenderTargetNormal = nullptr;
	LXRenderTargetViewD3D11* RenderTargetSpecular = nullptr;
	LXRenderTargetViewD3D11* RenderTargetEmissive = nullptr;
		
	// Object list
	list<LXRenderCluster*> ListRenderClusterOpaques;
	list<LXRenderCluster*> ListRenderClusterTransparents;
};

