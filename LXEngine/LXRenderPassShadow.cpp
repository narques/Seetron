//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "LXActor.h"
#include "LXActorCamera.h"
#include "LXBBox.h"
#include "LXCore.h"
#include "LXCore.h"
#include "LXFrustum.h"
#include "LXActorLight.h"
#include "LXProject.h"
#include "LXRenderCluster.h"
#include "LXRenderClusterManager.h"
#include "LXRenderCommandList.h"
#include "LXRenderPass.h"
#include "LXRenderPassShadow.h"
#include "LXRenderPipelineDeferred.h"
#include "LXRenderTargetViewD3D11.h"
#include "LXRenderer.h"
#include "LXScene.h"
#include "LXTextureD3D11.h"
#include "LXViewport.h"
#include "LXWorldTransformation.h"
#include "LXMemory.h" // --- Must be the last included ---

namespace
{
	const uint kShadowMapWidth = 512;
	const uint kShadowMapHeight = 512;
	const uint kAtlasShadowMapWidth = 2048;
	const uint kAtlasShadowMapHeight = 2048;
};

LXRenderPassShadow::LXRenderPassShadow(LXRenderer* InRenderer):LXRenderPass(InRenderer)
{
	TextureDepth = make_unique<LXTextureD3D11>(kAtlasShadowMapWidth, kAtlasShadowMapHeight, DXGI_FORMAT_R24G8_TYPELESS);
	DepthStencilView = make_unique<LXDepthStencilViewD3D11>(TextureDepth.get());
	LXConstantBufferDataSpotLight ConstantBufferDataSpotLight;
	ConstantBufferSpotLight = make_unique<LXConstantBufferD3D11>(&ConstantBufferDataSpotLight, static_cast<int>(sizeof(LXConstantBufferDataSpotLight)));
	LXRenderPipeline* RenderPipeline = Renderer->GetRenderPipeline();
	RenderPipeline->AddToViewDebugger(L"View.ShadowMaps", TextureDepth.get(), ETextureChannel::ChannelR);
}

LXRenderPassShadow::~LXRenderPassShadow()
{
}

void LXRenderPassShadow::Render(LXRenderCommandList* RCL)
{
	if (!GetProject())
		return;

	LXRenderPipelineDeferred* RenderPipelineDeferred = dynamic_cast<LXRenderPipelineDeferred*>(Renderer->GetRenderPipeline());
	CHK(RenderPipelineDeferred);

	RCL->BeginEvent(L"Shadows");

	RCL->OMSetRenderTargets2(nullptr, DepthStencilView.get());
	RCL->ClearDepthStencilView(DepthStencilView.get());

	for (const LXRenderCluster* RenderClusterLight : RenderPipelineDeferred->_ListRenderClusterLights)
	{
		CHK(RenderClusterLight->Flags & ERenderClusterType::Light);

		if (RenderClusterLight->ConstantBufferDataSpotLight->CastShadow == false)
		{
			continue;
		}

		float x = RenderClusterLight->ConstantBufferDataSpotLight->ShadowMapCoords.x;
		float y = RenderClusterLight->ConstantBufferDataSpotLight->ShadowMapCoords.y;
		
		// Light to Camera
		LXActorLight* Light = dynamic_cast<LXActorLight*>(RenderClusterLight->Actor);
		CHK(Light);
		vec3f LightPosition = Light->GetMatrixWCS().GetOrigin();
		vec3f LightDirection = Light->GetMatrixWCS().GetVz() * -1.f;

		#pragma message("move actor volatile: move depuis le thread RT")
		LXActorCamera Camera(nullptr);
		Camera.SetPosition(LightPosition);
		Camera.SetDirection(LightDirection);
		Camera.SetFov(Light->GetSpotAngle());
		Camera.LookAt(1.0);

		// Camera to WorlTransformation
		LXWorldTransformation WorldTransformation;
		WorldTransformation.FromCamera(&Camera, -1, -1);
		LXMatrix MatrixVP = WorldTransformation.GetMatrixVP();

		LXFrustum Frustum;
		Frustum.Update(MatrixVP);

		list<LXRenderCluster*> ListRenderClusterOpaques;

		for (LXRenderCluster* RenderCluster : Renderer->RenderClusterManager->ListRenderClusters)
		{
			if (Frustum.IsBoxIn(RenderCluster->BBoxWorld.GetMin(), RenderCluster->BBoxWorld.GetMax()))
			{
				if (!RenderCluster->IsTransparent() && RenderCluster->CastShadow)
				{
					ListRenderClusterOpaques.push_back(RenderCluster);
				}
			}
		}

		RCL->RSSetViewports2(x, y, (float)kShadowMapWidth, (float)kShadowMapHeight);
		
		RenderClusterLight->LightView->View = Transpose(WorldTransformation.GetMatrixView());
		RenderClusterLight->LightView->Projection = Transpose(WorldTransformation.GetMatrixProjection());
		RenderClusterLight->LightView->ViewProjectionInv = Transpose(WorldTransformation.GetMatrixVPInv());
		RenderClusterLight->LightView->ProjectionInv = Transpose(WorldTransformation.GetMatrixProjectionInv());
		RenderClusterLight->LightView->ViewInv = Transpose(WorldTransformation.GetMatrixViewInv());
		RenderClusterLight->LightView->CameraPosition = vec4f(Camera.GetPosition(), 0.0f);
		RenderClusterLight->LightView->RendererSize = vec2f((float)Renderer->Width, (float)Renderer->Height);
			
		RCL->CBViewProjection = RenderPipelineDeferred->_CBViewProjection;
		RCL->UpdateSubresource4(RenderPipelineDeferred->_CBViewProjection->D3D11Buffer, RenderClusterLight->LightView);
		
		for (LXRenderCluster* RenderCluster : ListRenderClusterOpaques)
		{
			RenderCluster->Render(ERenderPass::Shadow, RCL);
		}

		RCL->VSSetShader(nullptr);
		RCL->HSSetShader(nullptr);
		RCL->DSSetShader(nullptr);
		RCL->GSSetShader(nullptr);
		RCL->PSSetShader(nullptr);
	}

	RCL->EndEvent();
}

void LXRenderPassShadow::Resize(uint Width, uint Height)
{
}
