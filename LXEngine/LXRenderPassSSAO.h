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

class LXRenderPassSSAO : public LXRenderPass
{

public:

	LXRenderPassSSAO(LXRenderer* InRenderer);
	virtual ~LXRenderPassSSAO();

	const LXTextureD3D11* GetOutputTexture() const { return _TextureAO; }

private:

	void Resize(uint Width, uint Height);
	void Render(LXRenderCommandList* RenderCommandList) override;
	void CreateBuffers(uint Width, uint Height);
	void DeleteBuffers();
	void RebuildShaders() override;

public:

	LXRenderPassGBuffer* RenderPassGBuffer;

private:

	LXTextureD3D11* _TextureAO = nullptr;
	LXRenderTargetViewD3D11* _RenderTargetAO = nullptr;

	LXTextureD3D11* _TextureBlur = nullptr;
	LXRenderTargetViewD3D11* _RenderTargetBlur = nullptr;

	LXShaderD3D11* _VertexShaderAO;
	LXShaderD3D11* _PixelShaderAO;

	LXShaderD3D11* _VertexShaderBlur;
	LXShaderD3D11* _PixelShaderBlurX;
	LXShaderD3D11* _PixelShaderBlurY;
};

