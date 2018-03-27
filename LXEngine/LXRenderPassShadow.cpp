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
};

LXRenderPassShadow::LXRenderPassShadow(LXRenderer* InRenderer):LXRenderPass(InRenderer)
{
	TextureDepth = make_unique<LXTextureD3D11>(kShadowMapWidth, kShadowMapHeight, DXGI_FORMAT_R24G8_TYPELESS);
	DepthStencilView = make_unique<LXDepthStencilViewD3D11>(TextureDepth.get());
	ConstantBufferSpotLight = make_unique<LXConstantBufferD3D11>(&ConstantBufferDataSpotLight, static_cast<int>(sizeof(LXConstantBufferDataSpotLight)));
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

	ArrayLights& Lights = GetProject()->GetLights();
	if (Lights.size() == 0)
		return;

	// Light to Camera
	LXActorLight* Light = *Lights.begin();
	vec3f LightPosition = Light->GetMatrixWCS().GetOrigin();
	vec3f LightDirection = Light->GetMatrixWCS().GetVz() * -1.f;

	#pragma message("move actor volatile: move depuis le thread RT")
	LXActorCamera Camera(nullptr);
	Camera.SetPosition(LightPosition);
	Camera.SetDirection(LightDirection);
	//Camera.SetDirection(vec3f(0.f, 0.05f, -0.95f));
	Camera.SetFov(Light->GetSpotAngle());
	Camera.LookAt(1.0);

	// Camera to WorlTransformation
	LXWorldTransformation WorldTransformation;
	WorldTransformation.FromCamera(&Camera, kShadowMapWidth, kShadowMapHeight);
	
	// Store the matrices for the deferred lighting
	LXMatrix MatrixLightView = WorldTransformation.GetMatrixView();
	LXMatrix MatrixLightProjection = WorldTransformation.GetMatrixProjection();
	LXMatrix MatrixVP = WorldTransformation.GetMatrixVP();

	// Prepare the light data for RenderPassLighting
	ConstantBufferDataSpotLight.MatrixLightView = Transpose(MatrixLightView);
	ConstantBufferDataSpotLight.MatrixLightProjection = Transpose(MatrixLightProjection);
	ConstantBufferDataSpotLight.LightPosition = LightPosition;
	ConstantBufferDataSpotLight.LightIntensity = Light->GetIntensity();
	ConstantBufferDataSpotLight.Angle = LX_DEGTORAD(Light->GetSpotAngle()) * 0.5f;
				
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
	
	RCL->BeginEvent(L"Shadows");
	RCL->RSSetViewports(kShadowMapWidth, kShadowMapHeight);
	RCL->OMSetRenderTargets2(nullptr, DepthStencilView.get());
	RCL->ClearDepthStencilView(DepthStencilView.get());

	// Set The ViewProjection 
	{
		static LXConstantBufferData0 CB0;
		CB0.View = Transpose(WorldTransformation.GetMatrixView());
		CB0.Projection = Transpose(WorldTransformation.GetMatrixProjection());
		CB0.ViewProjectionInv = Transpose(WorldTransformation.GetMatrixVPInv());
		CB0.ProjectionInv = Transpose(WorldTransformation.GetMatrixProjectionInv());
		CB0.ViewInv = Transpose(WorldTransformation.GetMatrixViewInv());
		CB0.CameraPosition = Camera.GetPosition();
		RCL->CBViewProjection = RenderPipelineDeferred->_CBViewProjection;
		RCL->UpdateSubresource4(RenderPipelineDeferred->_CBViewProjection->D3D11Buffer, &CB0);
	}

	for (LXRenderCluster* RenderCluster : ListRenderClusterOpaques)
	{
		RenderCluster->Render(ERenderPass::Shadow, RCL);
	}

	RCL->VSSetShader(nullptr);
	RCL->HSSetShader(nullptr);
	RCL->DSSetShader(nullptr);
	RCL->GSSetShader(nullptr);
	RCL->PSSetShader(nullptr);

	RCL->EndEvent();
}

void LXRenderPassShadow::Resize(uint Width, uint Height)
{
}

