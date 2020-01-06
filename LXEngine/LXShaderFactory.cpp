//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "LXShaderFactory.h"
#include "LXShaderManager.h"
#include "LXInputElementDescD3D11Factory.h"
#include "LXRenderPass.h"
#include "LXPlatform.h"
#include "LXFile.h"
#include "LXMaterial.h"
#include "LXMaterialD3D11.h"
#include "LXTexture.h"
#include "LXTextureD3D11.h"
#include "LXGraphMaterialToHLSLConverter.h"
#include "LXShader.h"
#include "LXMemory.h" // --- Must be the last included ---

namespace
{
	const char kConstantBufferInsertionPos[] = "//CONSTANT_BUFFER";
	const char kTexturesInsersionPos[] = "//TEXTURES";
}

LXShaderFactory::LXShaderFactory()
{
}

LXShaderFactory::~LXShaderFactory()
{
}

void LXShaderFactory::GenerateVertexShader(const LXFilepath& Filename, const LXMaterialD3D11* materialD3D11, ERenderPass renderPass, int layoutMask)
{
	LXGraphMaterialToHLSLConverter graphMaterialToHLSLConverer;
	LXStringA ShaderBuffer = graphMaterialToHLSLConverer.GenerateCode(materialD3D11, renderPass, EShader::VertexShader, layoutMask);

	//
	// Insert the constant buffer declaration.
	//

	if (materialD3D11->GetConstantBufferVS().HasData())
	{
		LXStringA constantBufferDecl = ConstantBufferToHLSL(materialD3D11->GetConstantBufferVS());
		int Start = ShaderBuffer.Find(kConstantBufferInsertionPos);
		if (Start == -1)
		{
			CHK(0);
		}
		else
		{
			Start += (int)strlen(kConstantBufferInsertionPos) + 1;
			ShaderBuffer.Insert(Start, constantBufferDecl.GetBuffer());
		}
	}

	//
	// Insert Textures and SamplerStates
	//

	if (materialD3D11->GetTexturesVS().size() > 0)
	{
		LXStringA texturesDecl = ListTexturesToHLSL(materialD3D11->GetTexturesVS(), EShader::VertexShader);
		int Start = ShaderBuffer.Find(kTexturesInsersionPos);
		if (Start == -1)
		{
			CHK(0);
		}
		else
		{
			Start += (int)strlen(kTexturesInsersionPos) + 1;
			ShaderBuffer.Insert(Start, texturesDecl.GetBuffer());
		}
	}


	//
	// Write to disk.
	//

	LXPlatform::DeleteFile(Filename);

	LXFile File;
	if (File.Open(Filename, "wt") == true)
	{
		File.Write(ShaderBuffer.GetBuffer(), ShaderBuffer.size());
		File.Close();
	}
	else
	{
		LogE(ShaderManager, L"Unable to open shader file: %s", Filename.GetBuffer());
	}
}

void LXShaderFactory::GeneratePixelShader(const LXFilepath& Filename, const LXMaterialD3D11* materialD3D11, ERenderPass renderPass)
{
	LXGraphMaterialToHLSLConverter graphMaterialToHLSLConverer;
	LXStringA ShaderBuffer = graphMaterialToHLSLConverer.GenerateCode(materialD3D11, renderPass, EShader::PixelShader);

	//
	// Insert the constant buffer declaration.
	//

	if (materialD3D11->GetConstantBufferPS().HasData())
	{
		LXStringA constantBufferDecl = ConstantBufferToHLSL(materialD3D11->GetConstantBufferPS());
		int Start = ShaderBuffer.Find(kConstantBufferInsertionPos);
		if (Start == -1)
		{
			CHK(0);
		}
		else
		{
			Start += (int)strlen(kConstantBufferInsertionPos) + 1;
			ShaderBuffer.Insert(Start, constantBufferDecl.GetBuffer());
		}
	}

	//
	// Insert Textures and SamplerStates
	//

	if (materialD3D11->GetTexturesPS().size() > 0)
	{
		LXStringA texturesDecl = ListTexturesToHLSL(materialD3D11->GetTexturesPS(), EShader::PixelShader);
		int Start = ShaderBuffer.Find(kTexturesInsersionPos);
		if (Start == -1)
		{
			CHK(0);
		}
		else
		{
			Start += (int)strlen(kTexturesInsersionPos) + 1;
			ShaderBuffer.Insert(Start, texturesDecl.GetBuffer());
		}
	}
	

	//
	// Write to disk.
	//
	
	LXPlatform::DeleteFile(Filename);

	LXFile File;
	if (File.Open(Filename, "wt") == true)
	{
		File.Write(ShaderBuffer.GetBuffer(), ShaderBuffer.size());
		File.Close();
	}
	else
	{
		LogE(ShaderManager, L"Unable to open shader file: %s", Filename.GetBuffer());
	}
}

LXString LXShaderFactory::GetRenderPassName(ERenderPass RenderPass)
{
	switch (RenderPass)
	{
	case ERenderPass::Depth: return L"_Depth"; break;
	case ERenderPass::Shadow: return L"_Shadow"; break;
	case ERenderPass::GBuffer: return L"_GBuffer"; break;
	case ERenderPass::Transparency: return L"_Transparency"; break;
	case ERenderPass::RenderToTexture : return L"_RenderToTexture"; break;
	default: CHK(0); return L""; break;
	}
}

LXString LXShaderFactory::GetShaderName(EShader Shader)
{
	switch (Shader)
	{
	case EShader::VertexShader: return L"_VS"; break;
	case EShader::HullShader: return L"_HS"; break;
	case EShader::DomainShader: return L"_DS"; break;
	case EShader::GeometryShader: return L"_GS"; break;
	case EShader::PixelShader: return L"_PS"; break;
	default: CHK(0); return L""; break;
	}
}

LXString LXShaderFactory::GetLayoutName(int LayoutMask)
{
	switch (LayoutMask)
	{
	case (int)EPrimitiveLayout::P: return L"_P"; break;
	case (int)EPrimitiveLayout::PT: return L"_PT"; break;
	case (int)EPrimitiveLayout::PN: return L"_PN"; break;
	case (int)EPrimitiveLayout::PNT: return L"_PNT"; break;
	case (int)EPrimitiveLayout::PNABT: return L"_PNABT"; break;
	case (int)EPrimitiveLayout::PNABTI: return L"_PNABTI"; break;
	default: CHK(0); return L""; break;
	}
}

LXStringA LXShaderFactory::ConstantBufferToHLSL(const LXConstantBuffer& ConstantBuffer)
{
	LXStringA HLSLDeclaration = "cbuffer ConstantBuffer_001\n";
	HLSLDeclaration += "{\n";

	for (const auto& VD : ConstantBuffer.GetVariables())
	{
		LXStringA TypeName;
		switch (VD.Type)
		{
		case EHLSLType::HLSL_float: TypeName = "float"; break;
		case EHLSLType::HLSL_float2: TypeName = "float2"; break;
		case EHLSLType::HLSL_float3: TypeName = "float3"; break;
		case EHLSLType::HLSL_float4: TypeName = "float4"; break;
		case EHLSLType::HLSL_Matrix: TypeName = "matrix"; break;
		default: CHK(0); return false;
		}

		HLSLDeclaration += "  " + TypeName + " " + VD.Name + ";\n";
	}

	uint padSize = ConstantBuffer.GetPadSize();

	HLSLDeclaration += "  float Pad_ConstantBuffer_001[" + LXStringA::Number((int)(padSize / 4)) + "];\n";

	/*
	switch (padSize)
	{
	case 4: HLSLDeclaration += "  float Pad_ConstantBuffer_001;\n"; break;
	case 8: HLSLDeclaration += "  float2 Pad_ConstantBuffer_001;\n"; break;
	case 12: HLSLDeclaration += "  float3 Pad_ConstantBuffer_001;\n"; break;
	case 24: HLSLDeclaration += "  float4 Pad_ConstantBuffer_001;\n"; break;
	default:CHK(0);
	}
	*/

	HLSLDeclaration += "};\n";
	return HLSLDeclaration;
}

LXStringA LXShaderFactory::ListTexturesToHLSL(const list<LXTexture*>& listTextures, EShader shader)
{
	LXStringA HLSLDeclaration;

	int pixelSlot = 0;
	int vertexSlot = 0;
	
	for (LXTexture* texture : listTextures)
	{
		if (shader == EShader::VertexShader)
		{
			LXStringA n = LXStringA::Number(vertexSlot++);
			HLSLDeclaration += "Texture2D texture" + n + " : register(vs, t" + n + ");\n";
			HLSLDeclaration += "SamplerState sampler" + n + " : register(vs, s" + n + ");\n\n";
		}
		else if (shader == EShader::PixelShader)
		{
			LXStringA n = LXStringA::Number(pixelSlot++);
			HLSLDeclaration += "Texture2D texture" + n + " : register(ps, t" + n + ");\n";
			HLSLDeclaration += "SamplerState sampler" + n + " : register(ps, s" + n + ");\n\n";
		}
		else
		{
			CHK(0);
		}
	}
		
	return HLSLDeclaration;
}

LXString LXShaderFactory::BuildShaderFilename(const LXString& BaseName, ERenderPass RenderPass, int LayoutMask, EShader Shader)
{
	LXString Filename = L"_";
	Filename += BaseName;
	Filename += GetRenderPassName(RenderPass);
	if (LayoutMask != -1)
	{
		Filename += GetLayoutName(LayoutMask);
	}
	Filename += GetShaderName(Shader);
	Filename += L".hlsl";
	return Filename;
}

void LXShaderFactory::GenerateShader(const LXMaterial* Material, ERenderPass RenderPass)
{
	// Build the shader filenames
	const LXString& BaseName = Material->GetFilepath().GetFilenameNoExt();
	const LXString vsFilename = BuildShaderFilename(BaseName, RenderPass, -1, EShader::VertexShader);
	const LXString hsFilename = BuildShaderFilename(BaseName, RenderPass, -1, EShader::HullShader);
	const LXString dsFilename = BuildShaderFilename(BaseName, RenderPass, -1, EShader::DomainShader);
	const LXString gsFilename = BuildShaderFilename(BaseName, RenderPass, -1, EShader::GeometryShader);
	const LXString psFilename = BuildShaderFilename(BaseName, RenderPass, -1, EShader::PixelShader);
}
