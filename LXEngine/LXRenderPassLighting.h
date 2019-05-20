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
class LXRenderPipelineDeferred;
class LXRenderTarget;
class LXShaderProgramBasic;
struct LXConstantBufferDataIBL;

class LXRenderPassLighting : public LXRenderPass
{

public:

	LXRenderPassLighting(LXRenderer* InRenderer);
	virtual ~LXRenderPassLighting();
	void RebuildShaders() override;
	void Render(LXRenderCommandList* RenderCommandList) override;
	void Resize(uint Width, uint Height) override;
	bool IsValid() const override;
	const LXTextureD3D11* GetOutputTexture() const;
	const LXTextureD3D11* GetTextureIBL() const { return TextureIBL; }
			
private:

	void CreateBuffers(uint Width, uint Height);
	void DeleteBuffers();

	void RenderIBL(LXRenderCommandList* r, const LXRenderPipelineDeferred* RenderPipelineDeferred);
	void RenderSpotLight(LXRenderCommandList* RenderCommandList, const LXRenderPipelineDeferred* RenderPipelineDeferred);
	void RenderDirectionalLight(LXRenderCommandList* RenderCommandList, const LXRenderPipelineDeferred* RenderPipelineDeferred);
	void RenderPointLight(LXRenderCommandList* RenderCommandList, const LXRenderPipelineDeferred* RenderPipelineDeferred);

public:

	// Basic Objects
	LXRenderTarget* RenderTargetDiffuse = nullptr;
	LXRenderTarget* RenderTargetSpecular = nullptr;
	LXRenderTarget* RenderTargetCompose = nullptr;

	list<LXRenderCluster*>* _ListRenderClusterLights = nullptr;

	// Test: texture reflection
	LXConstantBufferD3D11* ConstantBufferIBL = nullptr;
	LXConstantBufferDataIBL* ConstantBufferDataIBL = nullptr;
	
	LXTextureD3D11* TextureShadow = nullptr;
		
	// Ref.
	LXRenderPassGBuffer* RenderPassGBuffer;
	LXRenderPassShadow* RenderPassShadow;
	LXRenderPassSSAO* RenderPassSSAO;

private:

	// Project Ref.
	const LXTextureD3D11* TextureIBL = nullptr;

	unique_ptr<LXShaderProgramBasic> _shaderProgramIBLLight;
	unique_ptr<LXShaderProgramBasic> _shaderProgramSpotLight;
	unique_ptr<LXShaderProgramBasic> _shaderProgramDirectionalLight;
	unique_ptr<LXShaderProgramBasic> _shaderProgramPointLight;
	unique_ptr<LXShaderProgramBasic> _shaderProgramComposeLight;
	unique_ptr<LXShaderProgramBasic> _shaderProgramComposeLightNoSSAO;
};

