//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "LXMaterialD3D11.h"
#include "LXRenderCommandList.h"
#include "LXMaterial.h"
#include "LXShader.h"
#include "LXCore.h"
#include "LXTextureD3D11.h"
#include "LXBitmap.h"
#include "LXRenderer.h"
#include "LXGraphMaterialToHLSLConverter.h"
#include "LXCore.h"
#include "LXShaderManager.h"
#include "LXRenderPassDynamicTexture.h"
#include "LXRenderPassLighting.h"
#include "LXRenderPassTransparency.h"
#include "LXRenderPipelineDeferred.h"
#include "LXRenderCluster.h"
#include "LXRenderTargetViewD3D11.h"
#include "LXTexture.h"
#include "LXTextureD3D11.h"
#include "LXGraph.h"
#include <dxgiformat.h>
#include "LXDirectX11.h"
#include "LXLogger.h"
#include "LXConstantBufferD3D11.h"
#include "LXConstantBuffer.h"
#include "LXFile.h"
#include "LXStatistic.h"
#include "LXShaderFactory.h"
#include "LXMemory.h" // --- Must be the last included ---

LXMaterialD3D11::LXMaterialD3D11()
{
	LX_COUNTSCOPEINC(LXMaterialD3D11)
	CHK(IsRenderThread());
}

LXMaterialD3D11::~LXMaterialD3D11()
{
	LX_COUNTSCOPEDEC(LXMaterialD3D11)
	CHK(IsRenderThread());
	Release();
}

void LXMaterialD3D11::Release()
{
	ListVSTextures.clear();
	ListPSTextures.clear();

	LX_SAFE_DELETE(CBMaterialParemetersVS);
	LX_SAFE_DELETE(CBMaterialParemetersPS);

	ConstantBufferVS.Release();
	ConstantBufferPS.Release();
}

void LXMaterialD3D11::Render(ERenderPass RenderPass, LXRenderCommandList* RCL) const
{
	LXRenderer* Renderer = RCL->Renderer;

	CHK(_bValid);
		
	// ConstantBuffer
	// if (NeedUpdate
	//{
	//RCL->UpdateSubresource4(CBMaterialParemetersVS->D3D11Buffer, &MaterialParameters);
	//RCL->VSSetConstantBuffers(2, 1, CBMaterialParemetersVS);
	//}
	
	if (CBMaterialParemetersPS)
	{
		if (ConstantBufferPS.ValueHasChanged)
		{
			// TODO MUTEX
			RCL->UpdateSubresource4(CBMaterialParemetersPS->D3D11Buffer, ConstantBufferPS.GetData());
			ConstantBufferPS.ValueHasChanged = false;
		}
		RCL->VSSetConstantBuffers((uint)LXConstantBufferSlot::CB_Material_Data, 1, CBMaterialParemetersPS);
		RCL->PSSetConstantBuffers((uint)LXConstantBufferSlot::CB_Material_Data, 1, CBMaterialParemetersPS);
	}

	// Rasterizer states
	
	if (ShowWireframe)
		RCL->RSSetState(Renderer->D3D11RasterizerStateWireframe);
	else if (TwoSided)
		RCL->RSSetState(Renderer->D3D11RasterizerStateTwoSided);
	else
		RCL->RSSetState(Renderer->D3D11RasterizerState);

	//
	// Textures
	//

	// Reset current states
	/* Pas possible la pass Transparency bind la texture "ambiant" sur le slot 10
	for (int i = 0; i < 16; i++)
	{
		//RCL->VSSetSamplers(i, 1, nullptr);
		RCL->VSSetShaderResources(i, 1, nullptr);
		//RCL->PSSetSamplers(i, 1, nullptr); // DOIT DUPPOER TABLEAU DE 1 NULL ( voir PSSetShaderReopusce= )
		RCL->PSSetShaderResources(i, 1, nullptr);
	}
	*/

	uint vertexSlot = 0;
	uint pixelSlot = 0;

	for (LXTexture* texture : ListVSTextures)
	{
		if (const LXTextureD3D11* textureD3D11 = texture->GetDeviceTexture())
		{
			RCL->VSSetSamplers(vertexSlot, 1, textureD3D11);
			RCL->VSSetShaderResources(vertexSlot, 1, textureD3D11);
			vertexSlot++;
		}
	}
		
	if (RenderPass == ERenderPass::GBuffer)
	{
		// User Textures
		for (LXTexture* texture : ListPSTextures)
		{
			if (const LXTextureD3D11* textureD3D11 = texture->GetDeviceTexture())
			{
				RCL->PSSetSamplers(pixelSlot, 1, textureD3D11);
				RCL->PSSetShaderResources(pixelSlot, 1, textureD3D11);
				pixelSlot++;
			}
		}

		// "System" textures (IBL,...) requested by the current pipeline
		// TODO: Binded only when needed
		//if (RenderPass == ERenderPass::Transparency)
		{
			// TODO: To Replace by a function like GetPassAdditionnalTexture or Pipeline->GetAdditionnalTextureToBind() or GetCurrentIBL()
			LXRenderPipelineDeferred* RenderPipelineDeferred = dynamic_cast<LXRenderPipelineDeferred*>(Renderer->GetRenderPipeline());

			if (const LXTextureD3D11* Texture = RenderPipelineDeferred->GetRenderPassLighting()->GetTextureIBL())
			{
				uint Slot = (uint)LXTextureSlot::Material_IBL;
				RCL->PSSetSamplers(Slot, 1, Texture);
				RCL->PSSetShaderResources(Slot, 1, Texture);
			}

			RCL->PSSetConstantBuffers((uint)LXConstantBufferSlot::CB_Material_IBL, 1, RenderPipelineDeferred->GetRenderPassTransparency()->CBImageBaseLighting.get());
		}
	}
}

void LXMaterialD3D11::Update(const LXMaterial* Material)
{
	//
	// Update the ConstantBuffer data values.
	//

	UpdateConstantBufferDataValues();

	//
	// Misc properties
	//

	if (TwoSided != Material->GetTwoSided())
	{
		TwoSided = Material->GetTwoSided();
	}

	if (Transparent != Material->IsTransparent())
	{
		Transparent = Material->IsTransparent();
	}

}

LXMaterialD3D11* LXMaterialD3D11::CreateFromMaterial(const LXMaterial* material)
{
	LXMaterialD3D11* materialD3D11 = new LXMaterialD3D11();
	CHK(materialD3D11->Create(material));
	return materialD3D11;
}

bool LXMaterialD3D11::Create(const LXMaterial* InMaterial)
{
	// Release Previous Objects
	Release();
	
	CHK(InMaterial);
	//CHK(Material == nullptr);

	Material = InMaterial;
	
	ComputeNormals = 0;
	//if (Material->GetComputeNormals())
	//	ComputeNormals = 1;
	
	TwoSided = Material->GetTwoSided() == true;
	Transparent = Material->IsTransparent();

	LXGraphMaterialToHLSLConverter graphMaterialToHLSLConverter;

	//
	// Build the VS Constant Buffer 
	//

	VRF(graphMaterialToHLSLConverter.GenerateConstanBuffer((const LXGraph*)Material->GetGraph(), EShader::VertexShader, ConstantBufferVS));
	VRF(graphMaterialToHLSLConverter.GatherTextures((const LXGraph*)Material->GetGraph(), EShader::VertexShader, ListVSTextures));

	if (ConstantBufferVS.HasData())
	{
		// Create the D3D11 Constant Buffer
		CBMaterialParemetersVS = new LXConstantBufferD3D11();
		CBMaterialParemetersVS->CreateConstantBuffer(ConstantBufferVS.GetData(), ConstantBufferVS.GetSize());
	}

	
	//
	// Build the PS Constant Buffer structure according the MaterialNodes
	//

	VRF(graphMaterialToHLSLConverter.GenerateConstanBuffer((const LXGraph*)Material->GetGraph(), EShader::PixelShader, ConstantBufferPS));
	VRF(graphMaterialToHLSLConverter.GatherTextures((const LXGraph*)Material->GetGraph(), EShader::PixelShader, ListPSTextures));

	if (ConstantBufferPS.HasData())
	{
		// Create the D3D11 Constant Buffer
		CBMaterialParemetersPS = new LXConstantBufferD3D11();
		CBMaterialParemetersPS->CreateConstantBuffer(ConstantBufferPS.GetData(), ConstantBufferPS.GetSize());
	}
	
	//
	// Textures and TextureSamplers
	//
		
	LogI(MaterialD3D11, L"Successful create %s", Material->GetName().GetBuffer());
	_bValid = true;
	return true;
}

void LXMaterialD3D11::UpdateConstantBufferDataValues()
{
	ConstantBufferVS.UpdateAll();
	ConstantBufferPS.UpdateAll();
}


