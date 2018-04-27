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
class LXRenderPassSSAO;

class LXRenderPassLighting : public LXRenderPass
{

public:

	LXRenderPassLighting(LXRenderer* InRenderer);
	virtual ~LXRenderPassLighting();
	void RebuildShaders() override;
	void Render(LXRenderCommandList* RenderCommandList) override;
	void Resize(uint Width, uint Height) override;
	bool IsValid() const override;
	const LXTextureD3D11* GetOutputTexture() const { return TextureColor; }
	const LXTextureD3D11* GetTextureIBL() const { return TextureIBL; }
			
private:

	void CreateBuffers(uint Width, uint Height);
	void DeleteBuffers();

public:

	// Basic Objects
	LXTextureD3D11* TextureColor = nullptr;
	LXRenderTargetViewD3D11* RenderTargetColor = nullptr;

	// Test: texture reflection
	LXTextureD3D11* TextureIBL = nullptr;
	LXTextureD3D11* TextureShadow = nullptr;
	
	// Ref.
	LXRenderPassGBuffer* RenderPassGBuffer;
	LXRenderPassShadow* RenderPassShadow;
	LXRenderPassSSAO* RenderPassSSAO;

private:

	LXShaderD3D11* _VertexShader;
	LXShaderD3D11* _PixelShader;

};

