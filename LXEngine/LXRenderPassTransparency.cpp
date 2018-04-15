//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "LXRenderPassTransparency.h"
#include "LXConstantBufferD3D11.h"
#include "LXRenderer.h"
#include "LXTextureD3D11.h"
#include "LXRenderTargetViewD3D11.h"
#include "LXRenderCluster.h"
#include "LXRenderCommandList.h"
#include "LXRenderPassGBuffer.h"
#include "LXRenderPassLighting.h"
#include "LXRenderPipelineDeferred.h"
#include "LXMemory.h" // --- Must be the last included ---

LXRenderPassTransparency::LXRenderPassTransparency(LXRenderer* Renderer):LXRenderPass(Renderer)
{
	// Textures and Samplers
	CreateBuffers(Renderer->Width, Renderer->Height);

	CBImageBaseLightingData.Intensity = 1.f;
	CBImageBaseLighting = make_unique<LXConstantBufferD3D11>(&CBImageBaseLightingData, (int)sizeof(CBImageBaseLightingData));
}

LXRenderPassTransparency::~LXRenderPassTransparency()
{
	DeleteBuffers();
}

void LXRenderPassTransparency::CreateBuffers(uint Width, uint Height)
{
	DeleteBuffers();
#if LX_DEDICATED_TRANSPARENCY_BUFFER
	TextureColor = new LXTextureD3D11(Width, Height, DXGI_FORMAT_B8G8R8A8_TYPELESS);

	// RenderTargetViews and ShaderResourceViews (in TextureD3D11 objects)
	RenderTargetColor = new LXRenderTargetViewD3D11(TextureColor);
#endif
}

void LXRenderPassTransparency::DeleteBuffers()
{
#if LX_DEDICATED_TRANSPARENCY_BUFFER
	LX_SAFE_DELETE(RenderTargetColor);
	LX_SAFE_DELETE(TextureColor);
#endif
}

void LXRenderPassTransparency::Resize(uint Width, uint Height)
{
	CreateBuffers(Width, Height);
}

void LXRenderPassTransparency::Render(LXRenderCommandList* RCL)
{
	LXRenderPipelineDeferred* RenderPipelineDeferred = dynamic_cast<LXRenderPipelineDeferred*>(Renderer->GetRenderPipeline());
	CHK(RenderPipelineDeferred);

	RCL->BeginEvent(L"Transparency");

	if (_ListRenderClusterTransparents->empty())
	{
		return;
	}

#if LX_DEDICATED_TRANSPARENCY_BUFFER
	RCL->OMSetRenderTargets2(RenderTargetColor, Renderer->RenderPassGBuffer->DepthStencilView);
#else
	RCL->OMSetRenderTargets2(RenderPipelineDeferred->RenderPassLighting->RenderTargetColor, RenderPipelineDeferred->RenderPassGBuffer->DepthStencilView);
	RCL->OMSetBlendState(Renderer->GetBlendStateTransparent());
#endif
	RCL->RSSetViewports(Renderer->Width, Renderer->Height);
	//RCL->ClearRenderTargetView(RenderTargetColor);
			
	for (LXRenderCluster* RenderCluster : *_ListRenderClusterTransparents)
	{
		RenderCluster->Render(ERenderPass::GBuffer, RCL);
	}

	//
	// Reset some shaders and resources 
	//

	RCL->VSSetShaderResources(1, 1, nullptr);
	RCL->VSSetShader(nullptr);
	RCL->HSSetShader(nullptr);
	RCL->DSSetShader(nullptr);
	RCL->GSSetShader(nullptr);
	RCL->PSSetShader(nullptr);

	// Restore the default rasterizer state
	RCL->RSSetState(Renderer->GetDefaultRasterizerState());
	RCL->OMSetBlendState(Renderer->GetBlendStateOpaque());


	RCL->EndEvent();
}


