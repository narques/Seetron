//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXRenderPass.h"

class LXRenderPassGBuffer;
class LXConstantBufferD3D11;
class LXRenderPassShadow;

class LXRenderPassLighting : public LXRenderPass
{

public:

	LXRenderPassLighting(LXRenderer* InRenderer);
	virtual ~LXRenderPassLighting();
	void Render(LXRenderCommandList* RenderCommandList) override;
	void Resize(uint Width, uint Height) override;
	const LXTextureD3D11* GetOutputTexture() const { return TextureColor; }
	bool IsValid() const override;
	
private:

	void CreateBuffers(uint Width, uint Height);
	void DeleteBuffers();

public:

	// Basic Objects
	LXTextureD3D11* TextureColor = nullptr;
	LXRenderTargetViewD3D11* RenderTargetColor = nullptr;

	// Test: texture reflection
	LXTextureD3D11* TextureReflection = nullptr;
	LXTextureD3D11* TextureShadow = nullptr;
	
	// Ref.
	LXRenderPassGBuffer* RenderPassGBuffer;
	LXRenderPassShadow* RenderPassShadow;
};

