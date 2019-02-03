//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2019 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "LXRenderPassDepthOfField.h"
#include "LXCore.h"
#include "LXInputElementDescD3D11Factory.h"
#include "LXProject.h"
#include "LXRenderCommandList.h"
#include "LXRenderPassLighting.h"
#include "LXRenderPipelineDeferred.h"
#include "LXRenderTarget.h"
#include "LXRenderer.h"
#include "LXSettings.h"
#include "LXShaderD3D11.h"
#include "LXShaderProgramD3D11.h"
#include "LXMemory.h" // --- Must be the last included ---

namespace
{
	const wchar_t kDOFShaderFilename[] = L"DepthOfField.hlsl";
	const DXGI_FORMAT kFormat = DXGI_FORMAT_R16G16B16A16_FLOAT;
};

LXRenderPassDepthOfField::LXRenderPassDepthOfField(LXRenderer* renderer) : LXRenderPass(renderer)
{
	_shaderDOFX = make_unique<LXShaderProgramBasic>();
	_shaderDOFX->PixelShader->AddMacro("DIRX", "1");
	_shaderDOFY = make_unique<LXShaderProgramBasic>();
	_renderTarget = make_unique<LXRenderTarget>(renderer->Width, renderer->Height, kFormat);
	RebuildShaders();
}

LXRenderPassDepthOfField::~LXRenderPassDepthOfField()
{
}

void LXRenderPassDepthOfField::RebuildShaders()
{
	const LXArrayInputElementDesc& Layout = GetInputElementDescD3D11Factory().GetInputElement_PT();
	_shaderDOFX->CreateShaders(GetSettings().GetShadersFolder() + kDOFShaderFilename, &Layout[0], (uint)Layout.size());
	_shaderDOFY->CreateShaders(GetSettings().GetShadersFolder() + kDOFShaderFilename, &Layout[0], (uint)Layout.size());
}

void LXRenderPassDepthOfField::CreateBuffers(uint width, uint height)
{
	_renderTarget->CreateBuffers(width, height, kFormat, true);
}

void LXRenderPassDepthOfField::Render(LXRenderCommandList* r)
{
	if (!GetProject() || !GetProject()->DepthOfField)
		return;
		   	
	LXRenderPipelineDeferred* renderPipelineDeferred = dynamic_cast<LXRenderPipelineDeferred*>(Renderer->GetRenderPipeline());
	CHK(renderPipelineDeferred);

	const LXTextureD3D11* lightedScene = renderPipelineDeferred->RenderPassLighting->GetOutputTexture();
	const LXTextureD3D11* depthBuffer = renderPipelineDeferred->GetDepthBuffer();
	LXRenderTarget* renderTargetCompose = renderPipelineDeferred->RenderPassLighting->RenderTargetCompose;
	
	r->BeginEvent(L"DepthOfField");
	
	// X to local RenderTarget
	r->OMSetRenderTargets2(_renderTarget->RenderTargetViewD3D11, nullptr);
	_shaderDOFX->Render(r);
	r->PSSetShaderResources(0, 1, lightedScene);
	r->PSSetSamplers(0, 1, lightedScene);
	r->PSSetShaderResources(1, 1, depthBuffer);
	r->PSSetSamplers(1, 1, depthBuffer);
	Renderer->DrawScreenSpacePrimitive(r);
	r->PSSetShaderResources(0, 1, nullptr);

	// Y to lightedScene
	r->OMSetRenderTargets2(renderTargetCompose->RenderTargetViewD3D11, nullptr);
	_shaderDOFY->Render(r);
	r->PSSetShaderResources(0, 1, _renderTarget->TextureD3D11);
	r->PSSetSamplers(0, 1, _renderTarget->TextureD3D11);
	r->PSSetShaderResources(1, 1, depthBuffer);
	r->PSSetSamplers(1, 1, depthBuffer);
	Renderer->DrawScreenSpacePrimitive(r);
	
	r->PSSetShaderResources(0, 1, nullptr);
	r->PSSetShaderResources(1, 1, nullptr);
		
	r->EndEvent();
}

void LXRenderPassDepthOfField::Resize(uint width, uint height)
{
	CreateBuffers(width, height);
}

const LXTextureD3D11* LXRenderPassDepthOfField::GetOutputTexture() const
{
	LXRenderPipelineDeferred* renderPipelineDeferred = dynamic_cast<LXRenderPipelineDeferred*>(Renderer->GetRenderPipeline());
	CHK(renderPipelineDeferred);
	return renderPipelineDeferred->RenderPassLighting->RenderTargetCompose->TextureD3D11;
}
