//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "LXShaderResources.h"

// Seetron
#include "LXRenderCommandList.h"
#include "LXRenderPassLighting.h"
#include "LXRenderPassTransparency.h"
#include "LXRenderPipelineDeferred.h"
#include "LXRenderer.h"

void LXShaderTexturesD3D11::Render(ERenderPass renderPass, LXRenderCommandList* RCL) const
{
	const LXRenderer* renderer = RCL->Renderer;
	uint vertexSlot = 0;
	uint pixelSlot = 0;
	
	for (LXTextureD3D11* textureD3D11 : TexturesVS)
	{
		RCL->VSSetSamplers(vertexSlot, 1, renderer->GetSamplerStateTexturing());
		RCL->VSSetShaderResources(vertexSlot, 1, textureD3D11);
		vertexSlot++;
	}

	// TODO 
	if (renderPass == ERenderPass::GBuffer || renderPass == ERenderPass::RenderToTexture)
	{
		// User Textures
		for (LXTextureD3D11* textureD3D11 : TexturesPS)
		{
			RCL->PSSetSamplers(pixelSlot, 1, renderer->GetSamplerStateTexturing());
			RCL->PSSetShaderResources(pixelSlot, 1, textureD3D11);
			pixelSlot++;
		}

		// "System" textures (IBL,...) requested by the current pipeline
		// TODO: Binded only when needed
		//if (RenderPass == ERenderPass::Transparency)
		{
			// TODO: To Replace by a function like GetPassAdditionnalTexture or Pipeline->GetAdditionnalTextureToBind() or GetCurrentIBL()
			LXRenderPipelineDeferred* renderPipelineDeferred = dynamic_cast<LXRenderPipelineDeferred*>(renderer->GetRenderPipeline());

			if (const LXTextureD3D11* Texture = renderPipelineDeferred->GetRenderPassLighting()->GetTextureIBL())
			{
				uint Slot = (uint)LXTextureSlot::Material_IBL;
				RCL->PSSetSamplers(Slot, 1, renderer->GetSamplerStateTexturing());
				RCL->PSSetShaderResources(Slot, 1, Texture);
			}

			RCL->PSSetConstantBuffers((uint)LXConstantBufferSlot::CB_Material_IBL, 1, renderPipelineDeferred->GetRenderPassTransparency()->CBImageBaseLighting.get());
		}
	}
}
