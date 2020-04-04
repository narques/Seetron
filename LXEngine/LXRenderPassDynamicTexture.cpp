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

	for (LXRenderCluster* renderCluster : renderPipelineDeferred->GetRenderClusterRenderToTextures())
	{
		const LXMaterial* material = renderCluster->Material;

		if (material->MaterialType == EMaterialType::MaterialTypeTexture)
		{
			LXActor* actor = const_cast<LXActor*>(renderCluster->RenderData->GetActor());
			const LXActorRenderToTexture* actorRTT = static_cast<LXActorRenderToTexture*>(actor);

			if (const LXTexture* textureOutput = actorRTT->GetTexture())
			{
				if (const LXTextureD3D11* textureD3D11 = textureOutput->GetDeviceTexture())
				{
					ID3D11RenderTargetView* renderTargetView = textureD3D11->GetRenderTargetView();

					RCL->OMSetRenderTargets(renderTargetView);
					RCL->RSSetViewports(textureOutput->GetWidth(), textureOutput->GetHeight());
					RCL->ClearRenderTargetView3(renderTargetView);

					renderCluster->Render(ERenderPass::RenderToTexture, RCL);

					RCL->VSSetShader(nullptr);
					RCL->HSSetShader(nullptr);
					RCL->DSSetShader(nullptr);
					RCL->GSSetShader(nullptr);
					RCL->PSSetShader(nullptr);
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

	RCL->EndEvent();
}

void LXRenderPassDynamicTexture::Resize(uint Width, uint Height)
{
}
