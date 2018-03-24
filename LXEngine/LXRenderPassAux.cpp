//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "LXActorCamera.h"
#include "LXProject.h"
#include "LXRenderPassAux.h"
#include "LXRenderTargetViewD3D11.h"
#include "LXRenderer.h"
#include "LXRenderClusterManager.h"
#include "LXRenderCommandList.h"
#include "LXViewport.h"
#include "LXFrustum.h"
#include "LXMemory.h" // --- Must be the last included ---
#include "LXRenderPass.h"
#include "LXRenderTargetViewD3D11.h"
#include "LXRenderCluster.h"

LXRenderPassAux::LXRenderPassAux(LXRenderer* InRenderer) :LXRenderPass(InRenderer)
{
	_Width = Renderer->Width;
	_Height = Renderer->Height;

	// RenderTargetViews and ShaderResourceViews (in TextureD3D11 objects)
	TextureDepth = std::make_unique<LXTextureD3D11>(_Width, _Height, DXGI_FORMAT_R24G8_TYPELESS);
	//DepthStencilView = std::make_unique<LXDepthStencilViewD3D11>(TextureDepth);
	DepthStencilView = new LXDepthStencilViewD3D11(TextureDepth.get());
}

LXRenderPassAux::~LXRenderPassAux()
{
	LX_SAFE_DELETE(DepthStencilView);
}

void LXRenderPassAux::Render(LXRenderCommandList* RCL)
{
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

	ListRenderClusterOpaques.clear();
	
	if (1)
	{
		LXFrustum Frustum;
		Frustum.Update(MatrixVP);

		for (LXRenderCluster* RenderCluster : Renderer->RenderClusterManager->ListRenderClusters)
		{
			if (Frustum.IsBoxIn(RenderCluster->BBoxWorld.GetMin(), RenderCluster->BBoxWorld.GetMax()))
			{
// 				if (RenderCluster->IsTransparent())
// 				{
// 					ListRenderClusterTransparents.push_back(RenderCluster);
// 				}
// 				else
				{
					ListRenderClusterOpaques.push_back(RenderCluster);
				}
			}
		}
	}

	//
	// DepthPass
	//

	{
		//RCL->IASetInputLayout(VSDrawToDepth);
		//RCL->VSSetShader(VSDrawToDepth);
		//RCL->VSSetConstantBuffers(0, 1, CBViewProjection);
		//RCL->PSSetShader(PSDrawToDepth);
	}

	//
	// MainPass
	//

	RCL->BeginEvent(L"MainPass");

	static ID3D11RenderTargetView* RenderTargetViews[4] = { 0 };
// 	RenderTargetViews[0] = RenderTargetColor->D3D11RenderTargetView;
// 	RenderTargetViews[1] = RenderTargetNormal->D3D11RenderTargetView;
// 	RenderTargetViews[2] = RenderTargetSpecular->D3D11RenderTargetView;
// 	RenderTargetViews[3] = RenderTargetEmissive->D3D11RenderTargetView;
//	RCL->OMSetRenderTargets3(4, RenderTargetViews, DepthStencilView->D3D11DepthStencilView);

	RCL->ClearDepthStencilView(DepthStencilView/*.get()*/);
// 	RCL->ClearRenderTargetView(RenderTargetColor);
// 	RCL->ClearRenderTargetView(RenderTargetNormal);
// 	RCL->ClearRenderTargetView(RenderTargetSpecular);
// 	RCL->ClearRenderTargetView(RenderTargetEmissive);

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

void LXRenderPassAux::Resize(uint Width, uint Height)
{
	#pragma message("TODO")
}
