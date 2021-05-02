//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2019 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXRenderPass.h"

class LXRenderTarget;
class LXShaderProgramBasic;

class LXRenderPassDepthOfField : public LXRenderPass
{

public:

	LXRenderPassDepthOfField(LXRenderer* renderer);
	virtual ~LXRenderPassDepthOfField();
	void Render(LXRenderCommandList* renderCommandList) override;
	void Resize(uint width, uint height) override;
	const LXTextureD3D11* GetOutputTexture() const;

private:

	void CreateBuffers(uint Width, uint Height);
	void RebuildShaders() override;

private:

	std::unique_ptr<LXRenderTarget> _renderTarget;
	std::unique_ptr<LXShaderProgramBasic> _shaderDOFX;
	std::unique_ptr<LXShaderProgramBasic> _shaderDOFY;

};
