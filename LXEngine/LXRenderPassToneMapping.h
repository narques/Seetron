//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXRenderPass.h"

class LXRenderTarget;

class LXRenderPassToneMapping : public LXRenderPass
{

public:

	LXRenderPassToneMapping(LXRenderer* InRenderer);
	virtual ~LXRenderPassToneMapping();
	void RebuildShaders() override;
	void Render(LXRenderCommandList* RenderCommandList) override;
	void Resize(uint Width, uint Height) override;
	bool IsValid() const override;
	const LXTextureD3D11* GetOutputTexture() const override;

private:

	void CreateBuffers(uint Width, uint Height);
	void DeleteBuffers();
	
	LXRenderTarget* _RenderTarget;
	LXShaderD3D11* _VertexShader;
	LXShaderD3D11* _PixelShader;

};

