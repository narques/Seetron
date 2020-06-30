//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "LXRenderPassDynamicTexture.h"
#include "LXActorRenderToTexture.h"
#include "LXMaterial.h"
#include "LXRenderer.h"
#include "LXRenderCluster.h"
#include "LXRenderClusterManager.h"
#include "LXRenderCommandList.h"
#include "LXRenderPipelineDeferred.h"
#include "LXTexture.h"
#include "LXTextureD3D11.h"
#include "LXMemory.h" // --- Must be the last included ---

LXRenderPassDynamicTexture::LXRenderPassDynamicTexture(LXRenderer* InRenderer) :LXRenderPass(InRenderer)
{
}

LXRenderPassDynamicTexture::~LXRenderPassDynamicTexture()
{
}

void LXRenderPassDynamicTexture::Render(LXRenderCommandList* RCL)
{
	const LXRenderPipelineDeferred* renderPipelineDeferred = dynamic_cast<const LXRenderPipelineDeferred*>(Renderer->GetRenderPipeline());

	RCL->BeginEvent(L"DynamicTexture");

	//
	// 'OnEveryFrame'
	//

	for (LXRenderCluster* renderCluster : renderPipelineDeferred->GetRenderClusterRenderToTextures())
	{
		const LXActor* actor = const_cast<LXActor*>(renderCluster->RenderData->GetActor());
		const LXActorRenderToTexture* actorRTT = static_cast<const LXActorRenderToTexture*>(actor);
		if (actorRTT->GetRenderingDrive() == ERenderingDrive::OnEveryFrame)
		{
			Render(RCL, renderCluster);
		}
	} 

	//
	// 'OnDemand' 
	//

	auto itRenderData = _onDemandRenderData.begin();
	while(itRenderData != _onDemandRenderData.end())
	{
		const map<LXRenderData*, list<LXRenderCluster*>>& renderClusters = Renderer->RenderClusterManager->GetActors();
		const auto itRenderCluser = renderClusters.find(*itRenderData);
		if (itRenderCluser != renderClusters.end())
		{
			const list<LXRenderCluster*> rdrc = itRenderCluser->second;
			for (LXRenderCluster* rendercluster : rdrc)
			{
				Render(RCL, rendercluster);
			}

			// lifecycle
			const LXActorRenderToTexture* actorRTT = static_cast<const LXActorRenderToTexture*>((*itRenderData)->GetActor());
			actorRTT->CurrentFrame++;
			if (actorRTT->CurrentFrame >= actorRTT->FrameCount)
			{
				actorRTT->CurrentFrame = 0;
				itRenderData = _onDemandRenderData.erase(itRenderData);
				LXDelegate<>* delegateAllFrameRendered = &(const_cast<LXActorRenderToTexture*>(actorRTT))->AllFrameRendered;
				RCL->EnqueueInvokeDelegate(delegateAllFrameRendered);
				continue;
			}
		}

		itRenderData++;
	}

	RCL->EndEvent();
}

void LXRenderPassDynamicTexture::Render(LXRenderCommandList* RCL, LXRenderCluster* renderCluster)
{
	const LXMaterialBase* material = renderCluster->Material;

	if (material->GetType() == EMaterialType::MaterialTypeTexture)
	{
		LXActor* actor = const_cast<LXActor*>(renderCluster->RenderData->GetActor());
		LXActorRenderToTexture* actorRTT = static_cast<LXActorRenderToTexture*>(actor);

		if (const LXTexture* textureOutput = actorRTT->GetTexture().get())
		{
			if (const LXTextureD3D11* textureD3D11 = textureOutput->GetDeviceTexture())
			{
				ID3D11RenderTargetView* renderTargetView = textureD3D11->GetRenderTargetView();

				RCL->OMSetRenderTargets(renderTargetView);
				RCL->RSSetViewports(textureOutput->GetWidth(), textureOutput->GetHeight());

				if (actorRTT->ClearRenderTarget)
					RCL->ClearRenderTargetView3(renderTargetView);

				renderCluster->Render(ERenderPass::RenderToTexture, RCL);

				RCL->VSSetShader(nullptr);
				RCL->HSSetShader(nullptr);
				RCL->DSSetShader(nullptr);
				RCL->GSSetShader(nullptr);
				RCL->PSSetShader(nullptr);
				LXDelegate<>* delegateRT = &actorRTT->Rendered_RT;
				RCL->InvokeDelegate(delegateRT);
			}
			else
			{
				CHK_ONCE(0);
				LogD(LXRenderPassDynamicTexture, L"LXActorRenderToTexture texture is not set.")
			}
		}
		else
		{
			CHK_ONCE(0);
			LogD(LXRenderPassDynamicTexture, L"LXActorRenderToTexture texture is not set.")
		}
	}
	else
	{
		CHK_ONCE(0);
		LogD(LXRenderPassDynamicTexture, L"Material is not a MaterialTypeTexture.")
	}
}

void LXRenderPassDynamicTexture::Resize(uint Width, uint Height)
{
}

void LXRenderPassDynamicTexture::AwakeActor(LXRenderData* renderData)
{
	CHK(IsRenderThread());
	_onDemandRenderData.push_back(renderData);
}
