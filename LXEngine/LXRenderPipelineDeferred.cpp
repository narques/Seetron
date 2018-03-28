//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "LXRenderPipelineDeferred.h"
#include "LXActorCamera.h"
#include "LXFrustum.h"
#include "LXProject.h"
#include "LXRenderCluster.h"
#include "LXRenderClusterManager.h"
#include "LXRenderer.h"
#include "LXRenderPassAux.h"
#include "LXRenderPassDownsample.h"
#include "LXRenderPassDynamicTexture.h"
#include "LXRenderPassGBuffer.h"
#include "LXRenderPassLighting.h"
#include "LXRenderPassShadow.h"
#include "LXRenderPassToneMapping.h"
#include "LXRenderPassTransparency.h"
#include "LXRenderPassUI.h"
#include "LXViewport.h"
#include "LXMemory.h" // --- Must be the last included ---

LXRenderPipelineDeferred::LXRenderPipelineDeferred(LXRenderer* Renderer):_Renderer(Renderer)
{
	// ConstantBuffers
	_CBViewProjection = new LXConstantBufferD3D11();
	LXConstantBufferData0 Foo;
	_CBViewProjection->CreateConstantBuffer(&Foo, sizeof(LXConstantBufferData0));
	
	// Passes
	RenderPassShadow = new LXRenderPassShadow(Renderer);
	RenderPassDynamicTexture = new LXRenderPassDynamicTexture(Renderer);
	RenderPassGBuffer = new LXRenderPassGBuffer(Renderer);
	RenderPassAux = new LXRenderPassAux(Renderer);
	RenderPassTransparent = new LXRenderPassTransparency(Renderer);
	RenderPassLighting = new LXRenderPassLighting(Renderer);
	RenderPassToneMapping = new LXRenderPassToneMapping(Renderer);
	RenderPassDownsample = new LXRenderPassDownsample(Renderer, EDownsampleFunction::Downsample_HalfRes);
	RenderPassUI = new LXRenderPassUI(Renderer);

	_RenderPasses.push_back(RenderPassShadow);
	_RenderPasses.push_back(RenderPassDynamicTexture);
	_RenderPasses.push_back(RenderPassGBuffer);
	//RenderPasses.push_back(RenderPassAux);
	_RenderPasses.push_back(RenderPassLighting);
	_RenderPasses.push_back(RenderPassTransparent);
	//RenderPasses.push_back(RenderPassDownsample);
	_RenderPasses.push_back(RenderPassToneMapping);
		
	// Links and references between objects
	RenderPassGBuffer->Viewport = Renderer->Viewport;
	RenderPassGBuffer->RenderPassShadow = RenderPassShadow;
	RenderPassGBuffer->_ListRenderClusterOpaques = &_ListRenderClusterOpaques;
	RenderPassAux->Viewport = Renderer->Viewport;
	RenderPassLighting->RenderPassGBuffer = RenderPassGBuffer;
	RenderPassLighting->RenderPassShadow = RenderPassShadow;
	RenderPassLighting->TextureShadow = RenderPassShadow->TextureDepth.get();
	RenderPassTransparent->_ListRenderClusterTransparents = &_ListRenderClusterTransparents;
}

LXRenderPipelineDeferred::~LXRenderPipelineDeferred()
{
	LX_SAFE_DELETE(RenderPassShadow);
	LX_SAFE_DELETE(RenderPassDynamicTexture);
	LX_SAFE_DELETE(RenderPassGBuffer);
	LX_SAFE_DELETE(RenderPassAux);
	LX_SAFE_DELETE(RenderPassTransparent);
	LX_SAFE_DELETE(RenderPassLighting);
	LX_SAFE_DELETE(RenderPassToneMapping);
	LX_SAFE_DELETE(RenderPassDownsample);
	LX_SAFE_DELETE(RenderPassUI);
	LX_SAFE_DELETE(_CBViewProjection);
}

void LXRenderPipelineDeferred::RebuildShaders()
{
	for (LXRenderPass* RenderPass : _RenderPasses)
	{
		RenderPass->RebuildShaders();
	}
}

void LXRenderPipelineDeferred::Resize(uint Width, uint Height)
{
	for (LXRenderPass* RenderPass : _RenderPasses)
	{
		RenderPass->Resize(Width, Height);
	}
}

void LXRenderPipelineDeferred::BuildRenderClusterLists()
{
	_ListRenderClusterOpaques.clear();
	_ListRenderClusterTransparents.clear();

	const LXProject* Project = _Renderer->GetProject();
	if (!Project)
		return;

	LXActorCamera* Camera = Project->GetCamera();
	if (!Camera)
		return;

	LXViewport* Viewport = _Renderer->Viewport;
				
	LXWorldTransformation* WorldTransformation = &Viewport->WorldTransformation;
	
	// Compute ViewMatrix
	Camera->LookAt(Viewport->GetAspectRatio());

	// Compute ProjectionMatrix
	WorldTransformation->FromCamera(Camera, _Renderer->Width, _Renderer->Height);

	//
	// Frustum Culling
	//

	const LXMatrix& MatrixVP = WorldTransformation->GetMatrixVP();
	LXFrustum Frustum;
	Frustum.Update(MatrixVP);

	for (LXRenderCluster* RenderCluster : _Renderer->RenderClusterManager->ListRenderClusters)
	{
		if (Frustum.IsBoxIn(RenderCluster->BBoxWorld.GetMin(), RenderCluster->BBoxWorld.GetMax()))
		{
			if (RenderCluster->IsTransparent())
			{
				_ListRenderClusterTransparents.push_back(RenderCluster);
			}
			else
			{
				_ListRenderClusterOpaques.push_back(RenderCluster);
			}
		}
	}

	//
	// Prepare
	// 

	_CBViewProjectionData.View = Transpose(WorldTransformation->GetMatrixView());
	_CBViewProjectionData.Projection = Transpose(WorldTransformation->GetMatrixProjection());
	_CBViewProjectionData.ViewProjectionInv = Transpose(WorldTransformation->GetMatrixVPInv());
	_CBViewProjectionData.ProjectionInv = Transpose(WorldTransformation->GetMatrixProjectionInv());
	_CBViewProjectionData.ViewInv = Transpose(WorldTransformation->GetMatrixViewInv());
	_CBViewProjectionData.CameraPosition = Camera->GetPosition();

}

void LXRenderPipelineDeferred::Render(LXRenderCommandList* RenderCommandList)
{
	// Creates and sorts the RenderCluster lists
	BuildRenderClusterLists();
	
	
	__super::Render(RenderCommandList);
}

void LXRenderPipelineDeferred::PostRender()
{
	RenderPassUI->Render(nullptr);
}

const LXTextureD3D11* LXRenderPipelineDeferred::GetDepthBuffer() const
{
	return RenderPassGBuffer->TextureDepth;
}

const LXTextureD3D11* LXRenderPipelineDeferred::GetColorBuffer() const
{
	return RenderPassGBuffer->TextureColor;
}

const LXTextureD3D11* LXRenderPipelineDeferred::GetNormalBuffer() const
{
	return RenderPassGBuffer->TextureNormal;
}

const LXTextureD3D11* LXRenderPipelineDeferred::GetMRULBuffer() const
{
	return RenderPassGBuffer->TextureSpecular;
}

const LXTextureD3D11* LXRenderPipelineDeferred::GetEmissiveBuffer() const
{
	return RenderPassGBuffer->TextureEmissive;
}

const LXTextureD3D11* LXRenderPipelineDeferred::GetOutput() const
{
	return RenderPassToneMapping->GetOutputTexture();
}

