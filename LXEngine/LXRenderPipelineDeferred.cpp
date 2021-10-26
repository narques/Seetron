//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "pch.h"
#include "LXRenderPipelineDeferred.h"

// Seetron
#include "LXActorCamera.h"
#include "LXActorSceneCapture.h"
#include "LXFrustum.h"
#include "LXProject.h"
#include "LXRenderBatching.h"
#include "LXRenderCluster.h"
#include "LXRenderClusterManager.h"
#include "LXRenderCommandList.h"
#include "LXRenderData.h"
#include "LXRenderPassAux.h"
#include "LXRenderPassDepth.h"
#include "LXRenderPassDepthOfField.h"
#include "LXRenderPassDownsample.h"
#include "LXRenderPassDynamicTexture.h"
#include "LXRenderPassGBuffer.h"
#include "LXRenderPassLighting.h"
#include "LXRenderPassSSAO.h"
#include "LXRenderPassShadow.h"
#include "LXRenderPassToneMapping.h"
#include "LXRenderPassTransparency.h"
#include "LXRenderPassUI.h"
#include "LXRenderer.h"
#include "LXViewState.h"
#include "LXViewport.h"

namespace
{
	LXConsoleCommandT<bool> Batching(L"Engine.ini", L"Renderer", L"Batching", L"true");
}

LXRenderPipelineDeferred::LXRenderPipelineDeferred(LXRenderer* Renderer):_Renderer(Renderer)
{
	_Renderer->_RenderPipeline = this;

	// ConstantBuffers
	_CBViewProjection = new LXConstantBufferD3D11();
	LXConstantBufferData0 Foo;
	_CBViewProjection->CreateConstantBuffer(&Foo, sizeof(LXConstantBufferData0));

	// Passes
	//RenderPassAA = new LXRenderPassAA(Renderer);
	RenderPassShadow = new LXRenderPassShadow(Renderer);
	RenderPassDynamicTexture = new LXRenderPassDynamicTexture(Renderer);
	RenderPassDepth = new LXRenderPassDepth(Renderer);
	RenderPassGBuffer = new LXRenderPassGBuffer(Renderer);
	RenderPassAux = new LXRenderPassAux(Renderer);
	RenderPassTransparent = new LXRenderPassTransparency(Renderer);
	RenderPassLighting = new LXRenderPassLighting(Renderer);
	RenderPassToneMapping = new LXRenderPassToneMapping(Renderer);
	RenderPassDownsample = new LXRenderPassDownsample(Renderer, EDownsampleFunction::Downsample_ToOne);
	RenderPassUI = new LXRenderPassUI(Renderer);
	RenderPassSSAO = new LXRenderPassSSAO(Renderer);
	_renderPassDOF = std::make_unique<LXRenderPassDepthOfField>(Renderer);
	   
	_RenderPasses.push_back(RenderPassDynamicTexture);
	_RenderPasses.push_back(RenderPassShadow);
	_RenderPasses.push_back(RenderPassDepth);
	_RenderPasses.push_back(RenderPassGBuffer);
	_RenderPasses.push_back(RenderPassAux);
	_RenderPasses.push_back(RenderPassSSAO);
	_RenderPasses.push_back(RenderPassLighting);
	_RenderPasses.push_back(RenderPassTransparent);
	_RenderPasses.push_back(_renderPassDOF.get());
	_RenderPasses.push_back(RenderPassDownsample);
	_RenderPasses.push_back(RenderPassToneMapping);
	//_RenderPasses.push_back(RenderPassAA);
		
	// Links and references between objects
	RenderPassGBuffer->Viewport = Renderer->Viewport;
	RenderPassGBuffer->RenderPassShadow = RenderPassShadow;
	RenderPassGBuffer->_ListRenderClusterOpaques = &_ListRenderClusterOpaques;
	RenderPassDepth->RenderClusters = &_ListRenderClusterOpaques;
	RenderPassAux->Viewport = Renderer->Viewport;
	RenderPassLighting->RenderPassGBuffer = RenderPassGBuffer;
	RenderPassLighting->RenderPassShadow = RenderPassShadow;
	RenderPassLighting->RenderPassSSAO = RenderPassSSAO;
	RenderPassLighting->TextureShadow = RenderPassShadow->TextureDepth.get();
	RenderPassLighting->_ListRenderClusterLights = &_ListRenderClusterLights;
	RenderPassTransparent->_ListRenderClusterTransparents = &_ListRenderClusterTransparents;
	RenderPassSSAO->RenderPassGBuffer = RenderPassGBuffer;
}

LXRenderPipelineDeferred::~LXRenderPipelineDeferred()
{
	LX_SAFE_DELETE(RenderPassShadow);
	LX_SAFE_DELETE(RenderPassDynamicTexture);
	LX_SAFE_DELETE(RenderPassGBuffer);
	LX_SAFE_DELETE(RenderPassDepth);
	LX_SAFE_DELETE(RenderPassAux);
	LX_SAFE_DELETE(RenderPassTransparent);
	LX_SAFE_DELETE(RenderPassLighting);
	LX_SAFE_DELETE(RenderPassToneMapping);
	LX_SAFE_DELETE(RenderPassDownsample);
	LX_SAFE_DELETE(RenderPassUI);
	//LX_SAFE_DELETE(RenderPassAA);
	LX_SAFE_DELETE(RenderPassSSAO);
	LX_SAFE_DELETE(_batching);
	LX_SAFE_DELETE(_CBViewProjection);
}

void LXRenderPipelineDeferred::Clear()
{
	if (_batching)
		_batching->Clear();
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
	_ListRenderClusterLights.clear();
	_ListRenderClusterAuxiliary.clear();
	_listRenderClusterRenderToTextures.clear();

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
		const bool IsLight = RenderCluster->Flags & ERenderClusterType::Light;
		const bool IsRenderToTexture = RenderCluster->Flags & ERenderClusterType::RenderToTexture;
		const bool ignoreFrustum = IsLight || IsRenderToTexture;

		if (ignoreFrustum || Frustum.IsBoxIn(RenderCluster->BBoxWorld.GetMin(), RenderCluster->BBoxWorld.GetMax()))
		{
			if (IsLight)
			{
				_ListRenderClusterLights.push_back(RenderCluster);
				if (RenderCluster->ConstantBufferDataSpotLight->CastShadow)
				{
					// Define the Shadow std::map position in the shadowAtlas
					GetTextureCoordinatesInAtlas(RenderCluster, RenderCluster->ConstantBufferDataSpotLight->ShadowMapCoords);
				}

				// For the Editor UI or Debug 
				//_ListRenderClusterAuxiliary.push_back(RenderCluster);
			}
			else if (RenderCluster->Flags & ERenderClusterType::Auxiliary)
			{
				_ListRenderClusterAuxiliary.push_back(RenderCluster);
			}
			else if (RenderCluster->IsTransparent())
			{
				_ListRenderClusterTransparents.push_back(RenderCluster);
			}
			else if (RenderCluster->Flags & ERenderClusterType::RenderToTexture)
			{
				_listRenderClusterRenderToTextures.push_back(RenderCluster);
			}
			else
			{
				RenderCluster->UpdateCurrentLOD(Camera->GetPosition());

				_ListRenderClusterOpaques.push_back(RenderCluster);

				if (RenderCluster->RenderData->ShowPrimitiveBBox())
					_ListRenderClusterAuxiliary.push_back(RenderCluster);
			}
		}
	}

	if (Batching.GetValue())
	{
		if (!_batching)
			_batching = new LXRenderBatching();
		_batching->Do(_ListRenderClusterOpaques);
	}
	else
	{
		if (_batching)
		{
			_batching->Clear();
			LX_SAFE_DELETE(_batching);
		}
	}
	
	//
	// Prepare ConstantBuffer Data
	// 

	_CBViewProjectionData.View = Transpose(WorldTransformation->GetMatrixView());
	_CBViewProjectionData.Projection = Transpose(WorldTransformation->GetMatrixProjection());
	_CBViewProjectionData.ViewProjectionInv = Transpose(WorldTransformation->GetMatrixVPInv());
	_CBViewProjectionData.ProjectionInv = Transpose(WorldTransformation->GetMatrixProjectionInv());
	_CBViewProjectionData.ViewInv = Transpose(WorldTransformation->GetMatrixViewInv());
	_CBViewProjectionData.CameraPosition = vec4f(Camera->GetPosition(), 0.f);
	_CBViewProjectionData.RendererSize = vec2f((float)_Renderer->Width, (float)_Renderer->Height);
	_CBViewProjectionData.Time = (float)GetEngine().Time.Time();
	_CBViewProjectionData.DeltaTime = (float)GetEngine().Time.DeltaTime();

	if (const LXActorSceneCapture* SceneCapture = GetEngine().GetProject()->GetSceneCapture())
	{
		RenderPassTransparent->CBImageBaseLightingData.Intensity = SceneCapture->GetIntensity();
	}
}

void LXRenderPipelineDeferred::Render(LXRenderCommandList* RenderCommandList)
{
	// Creates and sorts the RenderCluster lists
	// Prepare the ViewState (Camera) data
	BuildRenderClusterLists();

	// Update shared constant buffers
	RenderCommandList->UpdateSubresource4(RenderPassTransparent->CBImageBaseLighting->D3D11Buffer, &RenderPassTransparent->CBImageBaseLightingData);
			
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

const LXDepthStencilViewD3D11* LXRenderPipelineDeferred::GetDepthStencilView() const
{
	return RenderPassGBuffer->DepthStencilView;
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
	const LXTextureD3D11* Texture = RenderPassToneMapping->GetOutputTexture();
	
	if (GetProject())
	{
		if (LXViewState* ViewState = GetProject()->GetActiveView())
		{
			uint BufferID = ViewState->GetBufferToVisualize();
			if (BufferID > 0)
			{
				return _DebugTextures[BufferID - 1].TextureD3D11;
			}
		}
	}
	
	return Texture;
}

void LXRenderPipelineDeferred::GetTextureCoordinatesInAtlas(LXRenderCluster* RenderCluster, vec4f& outTextureCoordinates)
{
	static std::map < LXActor*, uint> mapActors;
	static bool IndexStatus[16] = { true };
	int static indices = 0;
	int index;

	LXActor* actor = const_cast<LXActor*>(RenderCluster->RenderData->GetActor());

	if (mapActors.find(actor) == mapActors.end())
	{
		index = indices;
		mapActors[actor] = index;
		indices++;
	}
	else 
	{
		index = mapActors[actor];
	}
		
	const float kShadowMapWidth = 512.f;
	const float kShadowMapHeight = 512.f;
	const float AtlasWidth = 2048.f;
	const float AtlasHeight = 2048.f;
	float y = (float)(index / 4);
	float x = index - (y * 4);
	outTextureCoordinates.x = x * kShadowMapWidth;
	outTextureCoordinates.y = y * kShadowMapHeight;
}

