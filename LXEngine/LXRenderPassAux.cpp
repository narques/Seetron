//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "LXActorCamera.h"
#include "LXFrustum.h"
#include "LXProject.h"
#include "LXRenderCluster.h"
#include "LXRenderCluster.h"
#include "LXRenderClusterManager.h"
#include "LXRenderCommandList.h"
#include "LXRenderPass.h"
#include "LXRenderPassAux.h"
#include "LXRenderPipelineDeferred.h"
#include "LXRenderTarget.h"
#include "LXRenderTargetViewD3D11.h"
#include "LXRenderer.h"
#include "LXViewport.h"
#include "LXMemory.h" // --- Must be the last included ---

LXRenderPassAux::LXRenderPassAux(LXRenderer* InRenderer) :LXRenderPass(InRenderer)
{
	CreateBuffers(Renderer->Width, Renderer->Height);
}

LXRenderPassAux::~LXRenderPassAux()
{
	DeleteBuffers();
}

void LXRenderPassAux::CreateBuffers(uint Width, uint Height)
{
	DeleteBuffers();
	_Depth = new LXRenderTargetDepth(Width, Height, DXGI_FORMAT_R24G8_TYPELESS);
	_Color = new LXRenderTarget(Width, Height, DXGI_FORMAT_B8G8R8A8_TYPELESS);

	LXRenderPipeline* RenderPipeline = Renderer->GetRenderPipeline();
	RenderPipeline->AddToViewDebugger(L"View.AuxiliaryDepth", _Depth->TextureD3D11, ETextureChannel::ChannelR);
	RenderPipeline->AddToViewDebugger(L"View.AuxiliaryColor", _Color->TextureD3D11, ETextureChannel::ChannelRGB);
}

void LXRenderPassAux::DeleteBuffers()
{
	LX_SAFE_DELETE(_Depth);
	LX_SAFE_DELETE(_Color);
}

void LXRenderPassAux::Render(LXRenderCommandList* RCL)
{
	const LXRenderPipelineDeferred* renderPipelineDeferred = dynamic_cast<const LXRenderPipelineDeferred*>(Renderer->GetRenderPipeline());

	RCL->BeginEvent(L"Auxiliary");
	RCL->OMSetRenderTargets2(_Color->RenderTargetViewD3D11, _Depth->RenderTargetViewD3D11);
	RCL->ClearDepthStencilView(_Depth->RenderTargetViewD3D11);
	RCL->ClearRenderTargetView(_Color->RenderTargetViewD3D11);

	for (LXRenderCluster* RenderCluster : renderPipelineDeferred->GetRenderClusterAuxiliary())
	{
		if (RenderCluster->Flags & ERenderClusterType::Surface)
			RenderCluster->RenderBounds(ERenderPass::GBuffer, RCL);
		else
			RenderCluster->Render(ERenderPass::GBuffer, RCL);
	}

	RCL->VSSetShader(nullptr);
	RCL->HSSetShader(nullptr);
	RCL->DSSetShader(nullptr);
	RCL->GSSetShader(nullptr);
	RCL->PSSetShader(nullptr);
	
	RCL->EndEvent();
}

void LXRenderPassAux::Resize(uint Width, uint Height)
{
	CreateBuffers(Width, Height);
}
