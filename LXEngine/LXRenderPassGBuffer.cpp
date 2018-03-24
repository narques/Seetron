//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "LXActorCamera.h"
#include "LXConstantBufferD3D11.h"
#include "LXDirectX11.h"
#include "LXFrustum.h"
#include "LXPrimitiveD3D11.h"
#include "LXProject.h"
#include "LXRenderCluster.h"
#include "LXRenderClusterManager.h"
#include "LXRenderCommandList.h"
#include "LXRenderPassGBuffer.h"
#include "LXRenderPassShadow.h"
#include "LXRenderTargetViewD3D11.h"
#include "LXRenderer.h"
#include "LXSettings.h"
#include "LXTextureD3D11.h"
#include "LXViewport.h"
#include "LXWorldTransformation.h"
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
	ListRenderClusterOpaques.clear();
	ListRenderClusterTransparents.clear();

	const LXProject* Project = Renderer->GetProject();

	if (!Project)
		return;

	//
	// Common
	//

	LXActorCamera* Camera = const_cast<LXActorCamera*>(Project->GetCamera());
	if (!Camera)
		return;

	LXWorldTransformation* WorldTransformation = &Viewport->WorldTransformation;

	// Compute ViewMatrix
	Camera->LookAt(Viewport->GetAspectRatio());

	// Compute ProjectionMatrix
	WorldTransformation->FromCamera(Camera, Renderer->Width, Renderer->Height);

	//LXMatrix MatrixVP = WorldTransformation->GetMatrixProjection() * WorldTransformation->GetMatrixView();
	LXMatrix MatrixVP = WorldTransformation->GetMatrixVP();

	//
	// Frustum Culling
	//

	if (1)
	{
		LXFrustum Frustum;
		Frustum.Update(MatrixVP);

		for (LXRenderCluster* RenderCluster : Renderer->RenderClusterManager->ListRenderClusters)
		{
			if (Frustum.IsBoxIn(RenderCluster->BBoxWorld.GetMin(), RenderCluster->BBoxWorld.GetMax()))
			{
				if (RenderCluster->IsTransparent())
				{
					ListRenderClusterTransparents.push_back(RenderCluster);
				}
				else
				{
					ListRenderClusterOpaques.push_back(RenderCluster);
				}
			}
		}
	}
		
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
	{
		RCL->CBViewProjection = Renderer->CBViewProjection;
		static LXConstantBufferData0 CB0;
		CB0.View = Transpose(WorldTransformation->GetMatrixView());
		CB0.Projection = Transpose(WorldTransformation->GetMatrixProjection());
		CB0.ViewProjectionInv = Transpose(WorldTransformation->GetMatrixVPInv());
		CB0.ProjectionInv = Transpose(WorldTransformation->GetMatrixProjectionInv());
		CB0.ViewInv = Transpose(WorldTransformation->GetMatrixViewInv());
		CB0.CameraPosition = Camera->GetPosition();
		RCL->UpdateSubresource4(Renderer->CBViewProjection->D3D11Buffer, &CB0);
	}

	for (LXRenderCluster* RenderCluster : ListRenderClusterOpaques)
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
	RCL->RSSetState(Renderer->D3D11RasterizerState);

	RCL->EndEvent();
}

void LXRenderPassGBuffer::Resize(uint Width, uint Height)
{
	CreateBuffers(Width, Height);
}
