//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "LXRenderPassSSAO.h"
#include "LXInputElementDescD3D11Factory.h"
#include "LXProject.h"
#include "LXRenderer.h"
#include "LXRenderCommandList.h"
#include "LXRenderPassGBuffer.h"
#include "LXRenderPipelineDeferred.h"
#include "LXRenderTargetViewD3D11.h"
#include "LXShaderD3D11.h"
#include "LXTextureD3D11.h"
#include "LXSettings.h"
#include "LXMemory.h" // --- Must be the last included ---

namespace
{
	const DXGI_FORMAT Format = DXGI_FORMAT_B8G8R8A8_TYPELESS; // DXGI_FORMAT_R8_TYPELESS;
	const wchar_t AOShaderFilename[] = L"SSAO.hlsl";
	const wchar_t BlurShaderFilename[] = L"Blur.hlsl";
};

LXRenderPassSSAO::LXRenderPassSSAO(LXRenderer* InRenderer): LXRenderPass(InRenderer)
{
	// Textures and Samplers
	CreateBuffers(Renderer->Width, Renderer->Height);

	_VertexShaderAO = new LXShaderD3D11();
	_PixelShaderAO = new LXShaderD3D11();

	_VertexShaderBlur = new LXShaderD3D11();
	_PixelShaderBlurX = new LXShaderD3D11();
	_PixelShaderBlurX->AddMacro("DIRX", "1");
	_PixelShaderBlurY = new LXShaderD3D11();

	RebuildShaders();
}

LXRenderPassSSAO::~LXRenderPassSSAO()
{
	DeleteBuffers();
	LX_SAFE_DELETE(_VertexShaderAO)
	LX_SAFE_DELETE(_PixelShaderAO)
	LX_SAFE_DELETE(_VertexShaderBlur)
	LX_SAFE_DELETE(_PixelShaderBlurX)
	LX_SAFE_DELETE(_PixelShaderBlurY)
}

void LXRenderPassSSAO::CreateBuffers(uint Width, uint Height)
{
	DeleteBuffers();
	_TextureAO = new LXTextureD3D11(lround(Width * 0.5), lround(Height * 0.5), Format);
	_RenderTargetAO = new LXRenderTargetViewD3D11(_TextureAO);

	_TextureBlur = new LXTextureD3D11(lround(Width * 0.5), lround(Height * 0.5), Format);
	_RenderTargetBlur = new LXRenderTargetViewD3D11(_TextureBlur);
}

void LXRenderPassSSAO::DeleteBuffers()
{
	LX_SAFE_DELETE(_RenderTargetAO);
	LX_SAFE_DELETE(_TextureAO);
	LX_SAFE_DELETE(_RenderTargetBlur);
	LX_SAFE_DELETE(_TextureBlur);
}

void LXRenderPassSSAO::RebuildShaders()
{
	const LXArrayInputElementDesc& Layout = GetInputElementDescD3D11Factory().GetInputElement_PT();
	
	_VertexShaderAO->CreateVertexShader(GetSettings().GetShadersFolder() + AOShaderFilename, &Layout[0], (uint)Layout.size());
	_PixelShaderAO->CreatePixelShader(GetSettings().GetShadersFolder() + AOShaderFilename);

	_VertexShaderBlur->CreateVertexShader(GetSettings().GetShadersFolder() + BlurShaderFilename, &Layout[0], (uint)Layout.size());
	_PixelShaderBlurX->CreatePixelShader(GetSettings().GetShadersFolder() + BlurShaderFilename);
	_PixelShaderBlurY->CreatePixelShader(GetSettings().GetShadersFolder() + BlurShaderFilename);
}

void LXRenderPassSSAO::Resize(uint Width, uint Height)
{
	CreateBuffers(Width, Height);
}

void LXRenderPassSSAO::Render(LXRenderCommandList* RCL)
{
	if (!Renderer->GetProject() || !Renderer->GetProject()->SSAO)
		return;

	LXRenderPipelineDeferred* RenderPipelineDeferred = dynamic_cast<LXRenderPipelineDeferred*>(Renderer->GetRenderPipeline());
	CHK(RenderPipelineDeferred);

	LXTextureD3D11* Depth = RenderPassGBuffer->TextureDepth;
	LXTextureD3D11* Normal = RenderPassGBuffer->TextureNormal;
	const LXTextureD3D11* Noise4x4 = Renderer->GetTextureNoise4x4();

	//
	// SSAO
	// 
	
	RCL->BeginEvent(L"AmbientOcclusion");
	RCL->OMSetRenderTargets2(_RenderTargetAO, nullptr);
	RCL->RSSetViewports(lround(Renderer->Width * 0.5), lround(Renderer->Height * 0.5));
	RCL->ClearRenderTargetView2(_RenderTargetAO, vec4f(1.f, 0.f, 0.f, 0.f));
	RCL->IASetInputLayout(_VertexShaderAO);
	RCL->VSSetShader(_VertexShaderAO);
	RCL->PSSetShader(_PixelShaderAO);
	RCL->PSSetConstantBuffers(0, 1, RenderPipelineDeferred->GetCBViewProjection());
	RCL->PSSetShaderResources(0, 1, (LXTextureD3D11*)Depth);
	RCL->PSSetShaderResources(2, 1, (LXTextureD3D11*)Normal);
	RCL->PSSetShaderResources(11, 1, (LXTextureD3D11*)Noise4x4);
	RCL->PSSetSamplers(0, 1, Renderer->GetSamplerStateRenderTarget());
	RCL->PSSetSamplers(2, 1, Renderer->GetSamplerStateRenderTarget());
	RCL->PSSetSamplers(11, 1, Renderer->GetSamplerStateTexturing());
	Renderer->DrawScreenSpacePrimitive(RCL);
	RCL->PSSetShaderResources(0, 1, nullptr);
	RCL->PSSetShaderResources(2, 1, nullptr);
	RCL->PSSetShaderResources(11, 1, nullptr);
	RCL->VSSetShader(nullptr);
	RCL->PSSetShader(nullptr);
	RCL->EndEvent();

	//
	// Blur
	// 

	RCL->BeginEvent(L"AmbientOcclusionSmooth");
	
	// X
	RCL->OMSetRenderTargets2(_RenderTargetBlur, nullptr);
	RCL->IASetInputLayout(_VertexShaderBlur);
	RCL->VSSetShader(_VertexShaderBlur);
	RCL->PSSetShader(_PixelShaderBlurX);
	RCL->PSSetShaderResources(0, 1, (LXTextureD3D11*)Depth);
	RCL->PSSetShaderResources(1, 1, (LXTextureD3D11*)_TextureAO);
	RCL->PSSetSamplers(0, 1, Renderer->GetSamplerStateRenderTarget());
	RCL->PSSetSamplers(1, 1, Renderer->GetSamplerStateRenderTarget());
	Renderer->DrawScreenSpacePrimitive(RCL);
	RCL->PSSetShaderResources(0, 1, nullptr);
	RCL->PSSetShaderResources(1, 1, nullptr);
	
	// Y
	RCL->OMSetRenderTargets2(_RenderTargetAO, nullptr);
	RCL->IASetInputLayout(_VertexShaderBlur);
	RCL->VSSetShader(_VertexShaderBlur);
	RCL->PSSetShader(_PixelShaderBlurY);
	RCL->PSSetShaderResources(0, 1, (LXTextureD3D11*)Depth);
	RCL->PSSetShaderResources(1, 1, (LXTextureD3D11*)_TextureBlur);
	RCL->PSSetSamplers(0, 1, Renderer->GetSamplerStateRenderTarget());
	RCL->PSSetSamplers(1, 1, Renderer->GetSamplerStateRenderTarget());
	Renderer->DrawScreenSpacePrimitive(RCL);
	RCL->PSSetShaderResources(0, 1, nullptr);
	RCL->PSSetShaderResources(1, 1, nullptr);
	RCL->VSSetShader(nullptr);
	RCL->PSSetShader(nullptr);
	RCL->EndEvent();
}
