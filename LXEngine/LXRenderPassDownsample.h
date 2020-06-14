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

namespace
{
	const int maxDownSample = 12;  // Enough for 4096x4096 to 1x1
}

enum class EDownsampleFunction
{
	Downsample_ToOne,
	Downsample_HalfRes
};

class LXRenderPassDownsample : public LXRenderPass
{

public:

	LXRenderPassDownsample(LXRenderer* InRenderer, EDownsampleFunction DownSampleFunction);
	virtual ~LXRenderPassDownsample();
	void Render(LXRenderCommandList* RenderCommandList) override;
	void Resize(uint Width, uint Height) override;
	bool IsValid() const override;

private:

	void CreateBuffers(uint Width, uint Height);
	void DeleteBuffers();

private:

	LXRenderTarget* _RenderTargerts[maxDownSample];
	LXShaderD3D11* _VertexShader;
	LXShaderD3D11* _PixelShader;
	
};

