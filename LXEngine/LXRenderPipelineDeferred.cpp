//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "LXRenderPipelineDeferred.h"
#include "LXAssetManager.h"
#include "LXActorCamera.h"
#include "LXFrustum.h"
#include "LXProject.h"
#include "LXRenderCluster.h"
#include "LXRenderClusterManager.h"
#include "LXRenderer.h"
//#include "LXRenderPassAA.h"
#include "LXRenderPassAux.h"
#include "LXRenderPassDownsample.h"
#include "LXRenderPassDynamicTexture.h"
#include "LXRenderPassGBuffer.h"
#include "LXRenderPassLighting.h"
#include "LXRenderPassShadow.h"
#include "LXRenderPassSSAO.h"
#include "LXRenderPassToneMapping.h"
#include "LXRenderPassTransparency.h"
#include "LXRenderPassUI.h"
#include "LXTextureManager.h"
#include "LXViewport.h"
#include "LXMemory.h" // --- Must be the last included ---
#include "LXRenderCommandList.h"

LXRenderPipelineDeferred::LXRenderPipelineDeferred(LXRenderer* Renderer):_Renderer(Renderer)
{
	// ConstantBuffers
	_CBViewProjection = new LXConstantBufferD3D11();
	LXConstantBufferData0 Foo;
	_CBViewProjection->CreateConstantBuffer(&Foo, sizeof(LXConstantBufferData0));
	
	// Passes
	//RenderPassAA = new LXRenderPassAA(Renderer);
	RenderPassShadow = new LXRenderPassShadow(Renderer);
	RenderPassDynamicTexture = new LXRenderPassDynamicTexture(Renderer);
	RenderPassGBuffer = new LXRenderPassGBuffer(Renderer);
	RenderPassAux = new LXRenderPassAux(Renderer);
	RenderPassTransparent = new LXRenderPassTransparency(Renderer);
	RenderPassLighting = new LXRenderPassLighting(Renderer);
	RenderPassToneMapping = new LXRenderPassToneMapping(Renderer);
	RenderPassDownsample = new LXRenderPassDownsample(Renderer, EDownsampleFunction::Downsample_HalfRes);
	RenderPassUI = new LXRenderPassUI(Renderer);
	RenderPassSSAO = new LXRenderPassSSAO(Renderer);

	_RenderPasses.push_back(RenderPassShadow);
	_RenderPasses.push_back(RenderPassDynamicTexture);
	_RenderPasses.push_back(RenderPassGBuffer);
	_RenderPasses.push_back(RenderPassSSAO);
	//RenderPasses.push_back(RenderPassAux);
	_RenderPasses.push_back(RenderPassLighting);
	_RenderPasses.push_back(RenderPassTransparent);
	//RenderPasses.push_back(RenderPassDownsample);
	_RenderPasses.push_back(RenderPassToneMapping);
	//_RenderPasses.push_back(RenderPassAA);
		
	// Links and references between objects
	RenderPassGBuffer->Viewport = Renderer->Viewport;
	RenderPassGBuffer->RenderPassShadow = RenderPassShadow;
	RenderPassGBuffer->_ListRenderClusterOpaques = &_ListRenderClusterOpaques;
	RenderPassAux->Viewport = Renderer->Viewport;
	RenderPassLighting->RenderPassGBuffer = RenderPassGBuffer;
	RenderPassLighting->RenderPassShadow = RenderPassShadow;
	RenderPassLighting->RenderPassSSAO = RenderPassSSAO;
	RenderPassLighting->TextureShadow = RenderPassShadow->TextureDepth.get();
	RenderPassTransparent->_ListRenderClusterTransparents = &_ListRenderClusterTransparents;
	RenderPassSSAO->RenderPassGBuffer = RenderPassGBuffer;
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
	//LX_SAFE_DELETE(RenderPassAA);
	LX_SAFE_DELETE(RenderPassSSAO);
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
	_CBViewProjectionData.CameraPosition = vec4f(Camera->GetPosition(), 0.f);
	_CBViewProjectionData.RendererSize = vec2f(_Renderer->Width, _Renderer->Height);
}

void LXRenderPipelineDeferred::Render(LXRenderCommandList* RenderCommandList)
{
	// Creates and sorts the RenderCluster lists
	// Prepare the ViewState (Camera) data
	BuildRenderClusterLists();


	if (GetProject())
	{
		if (LXTexture* Texture = GetAssetManager()->GetTexture(L"Textures/jittering4x4.stex"))
		{
			if (!_TextureNoise4x4)
			{
				_TextureNoise4x4 = RenderCommandList->Renderer->GetTextureManager()->GetTexture(Texture).get();
			}
		}
	}
			
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

