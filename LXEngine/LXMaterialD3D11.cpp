//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
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
#include "LXCore.h"
#include "LXShaderManager.h"
#include "LXRenderPassDynamicTexture.h"
#include "LXRenderPassLighting.h"
#include "LXRenderPassTransparency.h"
#include "LXRenderPipelineDeferred.h"
#include "LXRenderCluster.h"
#include "LXRenderTargetViewD3D11.h"
#include "LXTextureD3D11.h"
#include "LXGraph.h"
#include <dxgiformat.h>
#include "LXDirectX11.h"
#include "LXLogger.h"
#include "LXConstantBufferD3D11.h"
#include "LXConstantBuffer.h"
#include "LXFile.h"
#include "LXStatistic.h"
#include "LXMemory.h" // --- Must be the last included ---

LXMaterialD3D11::LXMaterialD3D11(const LXMaterial* InMaterial)
{
	LX_COUNTSCOPEINC(LXMaterialD3D11)
	Create(InMaterial);
}

LXMaterialD3D11::~LXMaterialD3D11()
{
	LX_COUNTSCOPEDEC(LXMaterialD3D11)
	Release();
}

void LXMaterialD3D11::Release()
{
	for (LXTextureD3D11* Texture : ListVSTextures)
	{
		delete Texture;
	}

	ListVSTextures.clear();

	for (LXTextureD3D11* Texture : ListPSTextures)
	{
		delete Texture;
	}

	ListPSTextures.clear();

	//LX_SAFE_DELETE(CBMaterialParemetersVS);
	LX_SAFE_DELETE(CBMaterialParemetersPS);

	//ConstantBufferVS.Release();
	ConstantBufferPS.Release();
}

void LXMaterialD3D11::Render(ERenderPass RenderPass, LXRenderCommandList* RCL)
{
	LXRenderer* Renderer = RCL->Renderer;

	// TODO : TMP : Not here: Once in rendering; objects should be valid ! 
	if (!_bValid)
	{
		Create(Material);
	}
	
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

	// Textures

	for (LXTextureD3D11* Texture : ListVSTextures)
	{
		uint Slot = Texture->Slot;
		RCL->VSSetSamplers(Slot, 1, Texture);
		RCL->VSSetShaderResources(Slot, 1, Texture);
	}
		
	if (RenderPass == ERenderPass::GBuffer)
	{
		// User Textures
		for (LXTextureD3D11* Texture : ListPSTextures)
		{
			if (Texture)
			{
				uint Slot = Texture->Slot;
				RCL->PSSetSamplers(Slot, 1, Texture);
				RCL->PSSetShaderResources(Slot, 1, Texture);
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
	Displacement = Material->Displacement != nullptr;
	Transparent = Material->IsTransparent();

	//
	// 
	//

	// Generated HLSL Code to inject into shader file
	LXStringA HLSLCodeVS;
	LXStringA HLSLCodePS;
		
	//
	// Build the VS Constant Buffer 
	//

//	Update(Material);
// 	CBMaterialParemetersVS = new LXConstantBufferD3D11();
// 	CBMaterialParemetersVS->CreateConstantBuffer(&MaterialParameters, sizeof(TMaterialParemeters));
	
	//
	// Build the PS Constant Buffer structure according the MaterialNodes
	//

	for (const LXMaterialNode* MaterialNode : Material->GetMaterialNodes())
	{
		if (LXMaterialNodeFloat* MaterialNodeFloat = CastMaterialNode<LXMaterialNodeFloat>(MaterialNode))
		{
			VRF(ConstantBufferPS.AddFloat(MaterialNodeFloat->GetName(), (const float&)MaterialNodeFloat->Value));
		}
		else if (LXMaterialNodeColor* MaterialNodeColor = CastMaterialNode<LXMaterialNodeColor>(MaterialNode))
		{
			VRF(ConstantBufferPS.AddFloat4(MaterialNodeColor->GetName(), (const vec4f&)MaterialNodeColor->Value));
		}
		else if (LXMaterialNodeTextureSampler* TextureSample = CastMaterialNode<LXMaterialNodeTextureSampler>(MaterialNode))
		{
			// Managed below 
		}
		else
		{
			CHK(0);
			LogE(MaterialD3D11, L"Unknown MaterialNode");
		}
	}

	if (ConstantBufferPS.HasData() && Material->PixelShader)
	{
		VRF(ConstantBufferPS.BuilldHLSL());
		HLSLCodePS += ConstantBufferPS.GetHLSLDeclaration();
		
		// Create the D3D11 Constant Buffer
		CBMaterialParemetersPS = new LXConstantBufferD3D11();
		CBMaterialParemetersPS->CreateConstantBuffer(ConstantBufferPS.GetData(), ConstantBufferPS.GetSize());
	}
	
	//
	// Textures and TextureSamplers
	//

	LXStringA HLSLTextureDeclarationVS;
	//int TextureCountVS = 0;

	LXStringA HLSLTextureDeclarationPS;
	//int TextureCountPS = 0;

	list<LXMaterialNodeTextureSampler*> listTextureSamplers;
	Material->GetTextureSamplers(listTextureSamplers);
	for (LXMaterialNodeTextureSampler* TextureSampler : listTextureSamplers)
	{
		if (!TextureSampler->GetTexture() || TextureSampler->AffectedShaders == 0)
		{
			continue;
		}

		// VertexShader
		if (TextureSampler->AffectedShaders & EShader::VertexShader)
		{
			ListVSTextures.push_back(CreateTexture(TextureSampler));
			//LXStringA n = LXStringA::Number(++TextureCountVS);
			LXStringA n = LXStringA::Number(TextureSampler->Slot);
			LXStringA HLSLTexture = "Texture2D texture" + n + " : register(vs, t" + n + ");\n";
			LXStringA HLSLSampler = "SamplerState sampler" + n + " : register(vs, s" + n + ");\n\n";
			HLSLTextureDeclarationVS += HLSLTexture;
			HLSLTextureDeclarationVS += HLSLSampler;
		}

		// PixelShader 
		if (TextureSampler->AffectedShaders & EShader::PixelShader)
		{
			ListPSTextures.push_back(CreateTexture(TextureSampler));
			//LXStringA n = LXStringA::Number(++TextureCountPS);
			LXStringA n = LXStringA::Number(TextureSampler->Slot);
			LXStringA HLSLTexture = "Texture2D texture" + n + " : register(ps, t" + n + ");\n";
			LXStringA HLSLSampler = "SamplerState sampler" + n + " : register(ps, s" + n + ");\n\n";
			HLSLTextureDeclarationPS += HLSLTexture;
			HLSLTextureDeclarationPS += HLSLSampler;
		}
	}

	HLSLCodeVS += HLSLTextureDeclarationVS;
	HLSLCodePS += HLSLTextureDeclarationPS;

	//
	// Inject
	// 

	// Only in custom shader. In the default Shader, declarations are hardcoded.
	if (Material->PixelShader)
	{
		const LXFilepath Filepath = Material->PixelShader->GetFilepath();
		LXStringA FileData;
		VRF(LXFileUtility::ReadTextFile(Filepath.GetBuffer(), FileData));
		int Start = FileData.Find("//BEGIN_GENERATED");
		int End = FileData.Find("//END_GENERATED");
		if (Start == -1 || End == -1)
		{
			LogW(MaterialD3D11, L"Unable to find //BEGIN_GENERATED or //END_GENERATED in shader %s.", Material->PixelShader->GetRelativeFilename().GetBuffer());
		}
		else
		{
			Start += 18; // the size of //BEGIN_GENERATED
			FileData.Erase(Start, End - Start);
			FileData.Insert(Start, HLSLCodePS.GetBuffer());
			VRF(LXFileUtility::WriteTextFile(Filepath.GetBuffer(), FileData));
		}
	}
		
	LogI(MaterialD3D11, L"Successful create %s", Material->GetName().GetBuffer());
	_bValid = true;
	return true;
}

LXTextureD3D11* LXMaterialD3D11::CreateTexture(LXMaterialNodeTextureSampler* TextureSampler)
{
	LXTexture* Texture = TextureSampler->GetTexture();
	if (!Texture)
	{
		LogE(MaterialD3D11, L"CreateTexture fails, the TextureSamplerNode is null");
		return nullptr;
	}
	
	const LXFilepath& Filepath = Texture->GetFilepath();
	
	LXTextureD3D11* TextureD3D11 = nullptr;

	if (Texture->TextureSource == ETextureSource::TextureSourceBitmap)
	{
		CHK(Texture->GetGraph() == nullptr);
		if (LXBitmap* Bitmap = Texture->GetBitmap(0))
		{
			DXGI_FORMAT Format = LXTextureD3D11::GetDXGIFormat(Bitmap->GetInternalFormat());
			uint MipLevels = LXBitmap::GetNumMipLevels(Bitmap->GetWidth(), Bitmap->GetHeight());
			TextureD3D11 = new LXTextureD3D11(Bitmap->GetWidth(), Bitmap->GetHeight(), Format, Bitmap->GetPixels(), Bitmap->GetPixelSize(), MipLevels);
		}
		else
		{
			LogE(MaterialD3D11, L"CreateTexture fails, Bitmap is null");
		}
	}
	else if (Texture->TextureSource == ETextureSource::TextureSourceDynamic)
	{
		CHK(Texture->GetGraph());
		CHK(0); // Find a more generic way to access the rendered texture
		//TextureD3D11 = GetCore().GetRenderer()->RenderPassDynamicTexture->AddGraph(Texture);
	}
	else
	{
		// Texture without source
		LogE(MaterialD3D11, L"CreateTexture fails, No source specified");
		CHK(0); 
	}
	
	if (TextureD3D11)
		TextureD3D11->Slot = TextureSampler->Slot;
	
	else
		LogE(MaterialD3D11, L"CreateTexture fails, unknown error");
		
	return TextureD3D11;
}

void LXMaterialD3D11::UpdateConstantBufferDataValues()
{
	for (const LXMaterialNode* MaterialNode : Material->GetMaterialNodes())
	{
		if (LXMaterialNodeFloat* MaterialNodeFloat = CastMaterialNode<LXMaterialNodeFloat>(MaterialNode))
		{
			ConstantBufferPS.Update(MaterialNodeFloat->GetName(), (const float&)MaterialNodeFloat->Value);
		}
		else if (LXMaterialNodeColor* MaterialNodeColor = CastMaterialNode<LXMaterialNodeColor>(MaterialNode))
		{
			ConstantBufferPS.Update(MaterialNodeColor->GetName(), (const vec4f&)MaterialNodeColor->Value);
		}
		else if (LXMaterialNodeTextureSampler* TextureSample = CastMaterialNode<LXMaterialNodeTextureSampler>(MaterialNode))
		{
			// Rebuild the Material
			_bValid = false;
		}
		else
		{
			CHK(0);
			LogE(MaterialD3D11, L"Unknown MaterialNode");
		}
	}
}
