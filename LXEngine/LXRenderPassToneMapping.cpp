//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "LXRenderPassToneMapping.h"
#include "LXRenderer.h"
#include "LXRenderTarget.h"
#include "LXTextureD3D11.h"
#include "LXRenderCommandList.h"
#include "LXRenderPassAux.h"
#include "LXRenderPassGBuffer.h"
#include "LXRenderPassLighting.h"
#include "LXRenderPipelineDeferred.h"
#include "LXShaderD3D11.h"
#include "LXInputElementDescD3D11Factory.h"
#include "LXSettings.h"
#include "LXMemory.h" // --- Must be the last included ---

namespace
{
	const wchar_t ShaderFilename[] = L"ToneMapping.hlsl";
	const DXGI_FORMAT Format = DXGI_FORMAT_B8G8R8A8_TYPELESS;
};

LXRenderPassToneMapping::LXRenderPassToneMapping(LXRenderer* InRenderer): LXRenderPass(InRenderer)
{
	uint Width = Renderer->Width;
	uint Height = Renderer->Height;
	
	_RenderTarget = new LXRenderTarget(Width, Height, Format);
	_VertexShader = new LXShaderD3D11();
	_PixelShader = new LXShaderD3D11();

	RebuildShaders();
}

LXRenderPassToneMapping::~LXRenderPassToneMapping()
{
	delete _RenderTarget;
	delete _VertexShader;
	delete _PixelShader;
}

void LXRenderPassToneMapping::RebuildShaders()
{
	const LXArrayInputElementDesc& Layout = GetInputElementDescD3D11Factory().GetInputElement_PT();
	_VertexShader->CreateVertexShader(GetSettings().GetShadersFolder() + ShaderFilename, &Layout[0], (uint)Layout.size());
	_PixelShader->CreatePixelShader(GetSettings().GetShadersFolder() + ShaderFilename);
}

bool LXRenderPassToneMapping::IsValid() const
{
	return _PixelShader->IsValid() && _VertexShader->IsValid();
}

const LXTextureD3D11* LXRenderPassToneMapping::GetOutputTexture() const
{
	return _RenderTarget->_TextureD3D11;
}

void LXRenderPassToneMapping::CreateBuffers(uint Width, uint Height)
{
	DeleteBuffers();
	_RenderTarget->CreateBuffers(Width, Height, Format);
}

void LXRenderPassToneMapping::DeleteBuffers()
{
	_RenderTarget->DeleteBuffers();
}

void LXRenderPassToneMapping::Render(LXRenderCommandList* r)
{
	LXRenderPipelineDeferred* RenderPipelineDeferred = dynamic_cast<LXRenderPipelineDeferred*>(Renderer->GetRenderPipeline());
	CHK(RenderPipelineDeferred);

	// Aux buffer contains 3D UI Items (Gizmo, LightIcons, Helpers, etc.)
	const LXTextureD3D11* SceneColor = RenderPipelineDeferred->RenderPassLighting->GetOutputTexture();
	const LXTextureD3D11* SceneDepth = RenderPipelineDeferred->GetDepthBuffer();
	const LXTextureD3D11* AuxColor = RenderPipelineDeferred->GetRenderPassAux()->GetColorRenderTarget()->_TextureD3D11;
	const LXTextureD3D11* AuxDepth = RenderPipelineDeferred->GetRenderPassAux()->GetDepthRenderTarget()->_TextureD3D11;

	r->BeginEvent(L"ToneMapping");
	r->OMSetRenderTargets2(_RenderTarget->_RenderTargetViewD3D11, nullptr);
	r->IASetInputLayout(_VertexShader);
	r->VSSetShader(_VertexShader);
	r->PSSetShader(_PixelShader);
	r->PSSetShaderResources(0, 1, SceneColor);
	r->PSSetShaderResources(1, 1, SceneDepth);
	r->PSSetShaderResources(2, 1, AuxColor);
	r->PSSetShaderResources(3, 1, AuxDepth);
	r->PSSetSamplers(0, 1, SceneColor);
	r->PSSetSamplers(1, 1, SceneDepth);
	r->PSSetSamplers(2, 1, AuxColor);
	r->PSSetSamplers(3, 1, AuxDepth);
	Renderer->DrawScreenSpacePrimitive(r);
	r->PSSetShaderResources(0, 1, nullptr);
	r->EndEvent();
}

void LXRenderPassToneMapping::Resize(uint Width, uint Height)
{
	CreateBuffers(Width, Height);
}
