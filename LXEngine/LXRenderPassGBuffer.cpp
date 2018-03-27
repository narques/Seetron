//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "LXRenderPassGBuffer.h"
#include "LXRenderCluster.h"
#include "LXRenderCommandList.h"
#include "LXRenderPipelineDeferred.h"
#include "LXRenderTargetViewD3D11.h"
#include "LXRenderer.h"
#include "LXTextureD3D11.h"
#include "LXMemory.h" // --- Must be the last included ---

LXRenderPassGBuffer::LXRenderPassGBuffer(LXRenderer* InRenderer):LXRenderPass(InRenderer)
{
	CreateBuffers(Renderer->Width, Renderer->Height);
 }

LXRenderPassGBuffer::~LXRenderPassGBuffer()
{
	DeleteBuffers();
}

void LXRenderPassGBuffer::CreateBuffers(uint Width, uint Height)
{
	DeleteBuffers();

	TextureDepth = new LXTextureD3D11(Width, Height, DXGI_FORMAT_R24G8_TYPELESS);
	TextureColor = new LXTextureD3D11(Width, Height, DXGI_FORMAT_B8G8R8A8_TYPELESS);
	TextureNormal = new LXTextureD3D11(Width, Height, DXGI_FORMAT_R10G10B10A2_UNORM);
	TextureSpecular = new LXTextureD3D11(Width, Height, DXGI_FORMAT_B8G8R8A8_TYPELESS);
	TextureEmissive  = new LXTextureD3D11(Width, Height, DXGI_FORMAT_R16G16B16A16_FLOAT);

	AddToViewDebugger(L"GBuffer Depth", TextureDepth, ETextureChannel::ChannelR);
	AddToViewDebugger(L"GBuffer Color", TextureColor, ETextureChannel::ChannelRGB);
	AddToViewDebugger(L"GBuffer Depth", TextureNormal, ETextureChannel::ChannelRGB);
	AddToViewDebugger(L"GBuffer Metal", TextureSpecular, ETextureChannel::ChannelR);
	AddToViewDebugger(L"GBuffer Rough", TextureSpecular , ETextureChannel::ChannelG);
	AddToViewDebugger(L"GBuffer LightModel", TextureSpecular, ETextureChannel::ChannelA);
	AddToViewDebugger(L"GBuffer Emissive", TextureEmissive, ETextureChannel::ChannelRGB);
	
	DepthStencilView = new LXDepthStencilViewD3D11(TextureDepth);
	RenderTargetColor = new LXRenderTargetViewD3D11(TextureColor);
	RenderTargetNormal = new LXRenderTargetViewD3D11(TextureNormal);
	RenderTargetSpecular = new LXRenderTargetViewD3D11(TextureSpecular);
	RenderTargetEmissive = new LXRenderTargetViewD3D11(TextureEmissive);
}

void LXRenderPassGBuffer::DeleteBuffers()
{
	LX_SAFE_DELETE(DepthStencilView);
	LX_SAFE_DELETE(RenderTargetColor);
	LX_SAFE_DELETE(RenderTargetNormal);
	LX_SAFE_DELETE(RenderTargetSpecular);
	LX_SAFE_DELETE(RenderTargetEmissive);

	LX_SAFE_DELETE(TextureDepth);
	LX_SAFE_DELETE(TextureColor);
	LX_SAFE_DELETE(TextureNormal);
	LX_SAFE_DELETE(TextureSpecular);
	LX_SAFE_DELETE(TextureEmissive);
}

void LXRenderPassGBuffer::Render(LXRenderCommandList* RCL)
{
	LXRenderPipelineDeferred* RenderPipelineDeferred = dynamic_cast<LXRenderPipelineDeferred*>(Renderer->GetRenderPipeline());
	CHK(RenderPipelineDeferred);
		
	//
	// MainPass
	//

	RCL->BeginEvent(L"MainPass");
	
	static ID3D11RenderTargetView* RenderTargetViews[4] = { 0 };
	RenderTargetViews[0] = RenderTargetColor->D3D11RenderTargetView;
	RenderTargetViews[1] = RenderTargetNormal->D3D11RenderTargetView;
	RenderTargetViews[2] = RenderTargetSpecular->D3D11RenderTargetView;
	RenderTargetViews[3] = RenderTargetEmissive->D3D11RenderTargetView;
	RCL->OMSetRenderTargets3(4, RenderTargetViews, DepthStencilView->D3D11DepthStencilView);

	RCL->RSSetViewports(Renderer->Width, Renderer->Height);
		
	RCL->ClearDepthStencilView(DepthStencilView);
	RCL->ClearRenderTargetView(RenderTargetColor);
	RCL->ClearRenderTargetView(RenderTargetNormal);
	RCL->ClearRenderTargetView(RenderTargetSpecular);
	RCL->ClearRenderTargetView(RenderTargetEmissive);
			
	// ConstantBuffers per Frame (View, Projection)
	RCL->CBViewProjection = RenderPipelineDeferred->_CBViewProjection;
	RCL->UpdateSubresource4(RenderPipelineDeferred->_CBViewProjection->D3D11Buffer, &RenderPipelineDeferred->_CBViewProjectionData);

	for (LXRenderCluster* RenderCluster : *_ListRenderClusterOpaques)
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

	RCL->EndEvent();
}

void LXRenderPassGBuffer::Resize(uint Width, uint Height)
{
	CreateBuffers(Width, Height);
}
