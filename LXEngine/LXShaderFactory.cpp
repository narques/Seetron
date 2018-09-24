//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
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

void LXShaderFactory::GenerateVertexShader(const LXFilepath& Filename, const LXString& FileToInclude, const LXVSSignature& VSSignature)
{
	LXStringA ShaderBuffer;

	// Common
	ShaderBuffer += "// *** Generated file ***\n";
	ShaderBuffer += "#include \"" + LXStringA(&*FileToInclude.GetBufferA().begin()) + "\"\n";
	ShaderBuffer += "\n";

	if (VSSignature.LayoutMask == (int)EPrimitiveLayout::P)
	{
		ShaderBuffer += "VS_OUTPUT VS(VS_INPUT_P input)\n";
		ShaderBuffer += "{\n";
		ShaderBuffer += "	VS_VERTEX Vertex = (VS_VERTEX)0;\n";
		ShaderBuffer += "	Vertex.Pos = input.Pos;\n";
		// Default values
		ShaderBuffer += "	Vertex.Normal = float3(0.0, 0.0, 1.0);\n";
		ShaderBuffer += "	Vertex.Tangent = float3(1.0, 0.0, 0.0);\n";
		ShaderBuffer += "	Vertex.Binormal = float3(0.0, 1.0, 0.0);\n";
		ShaderBuffer += "	Vertex.TexCoord = float2(0.0, 0.0);\n";
		ShaderBuffer += "	Vertex.InstanceID = 0;\n";
		ShaderBuffer += "	Vertex.InstancePos = float3(0.0, 0.0, 0.0);\n";
		ShaderBuffer += "	return ComputeVertex(Vertex);\n";
		ShaderBuffer += "}\n";

	}
	else if (VSSignature.LayoutMask == (int)EPrimitiveLayout::PT)
	{
		CHK(0);
	}
	else if (VSSignature.LayoutMask == (int)EPrimitiveLayout::PN)
	{
		ShaderBuffer += "VS_OUTPUT VS(VS_INPUT_PN input)\n";
		ShaderBuffer += "{\n";
		ShaderBuffer += "	VS_VERTEX Vertex = (VS_VERTEX)0;\n";
		ShaderBuffer += "	Vertex.Pos = input.Pos;\n";
		ShaderBuffer += "	Vertex.Normal = input.Normal; \n";
		// Default values
		ShaderBuffer += "	Vertex.Tangent = float3(1.0, 0.0, 0.0);\n";
		ShaderBuffer += "	Vertex.Binormal = float3(0.0, 1.0, 0.0);\n";
		ShaderBuffer += "	Vertex.TexCoord = float2(0.0, 0.0);\n";
		ShaderBuffer += "	Vertex.InstanceID = 0;\n";
		ShaderBuffer += "	Vertex.InstancePos = float3(0.0, 0.0, 0.0);\n";
		//
		ShaderBuffer += "	return ComputeVertex(Vertex);\n";
		ShaderBuffer += "}\n";
	}
	else if (VSSignature.LayoutMask == (int)EPrimitiveLayout::PNT)
	{
		ShaderBuffer += "VS_OUTPUT VS(VS_INPUT_PNT input)\n";
		ShaderBuffer += "{\n";
		ShaderBuffer += "	VS_VERTEX Vertex = (VS_VERTEX)0;\n";
		ShaderBuffer += "	Vertex.Pos = input.Pos;\n";
		ShaderBuffer += "	Vertex.Normal = input.Normal; \n";
		ShaderBuffer += "	Vertex.TexCoord = input.TexCoord;\n";
		// Default values
		ShaderBuffer += "	Vertex.Tangent = float3(1.0, 0.0, 0.0);\n";
		ShaderBuffer += "	Vertex.Binormal = float3(0.0, 1.0, 0.0);\n";
		ShaderBuffer += "	Vertex.InstanceID = 0;\n";
		ShaderBuffer += "	Vertex.InstancePos = float3(0.0, 0.0, 0.0);\n";
		//
		ShaderBuffer += "	return ComputeVertex(Vertex);\n";
		ShaderBuffer += "}\n";
	}
	else if (VSSignature.LayoutMask == (int)EPrimitiveLayout::PNABT)
	{
		ShaderBuffer += "VS_OUTPUT VS(VS_INPUT_PNABT input)\n";
		ShaderBuffer += "{\n";
		ShaderBuffer += "	VS_VERTEX Vertex = (VS_VERTEX)0;\n";
		ShaderBuffer += "	Vertex.Pos = input.Pos;\n";
		ShaderBuffer += "	Vertex.Normal = input.Normal; \n";
		ShaderBuffer += "	Vertex.Tangent = input.Tangent;\n";
		ShaderBuffer += "	Vertex.Binormal = input.Binormal;\n";
		ShaderBuffer += "	Vertex.TexCoord = input.TexCoord;\n";
		ShaderBuffer += "	Vertex.SupportNormalMap = true;\n";
		// Default values
		ShaderBuffer += "	Vertex.InstanceID = 0;\n";
		ShaderBuffer += "	Vertex.InstancePos = float3(0.0, 0.0, 0.0);\n";
		//
		ShaderBuffer += "	return ComputeVertex(Vertex);\n";
		ShaderBuffer += "}\n";
	}
	else if (VSSignature.LayoutMask == (int)EPrimitiveLayout::PNABTI)
	{
		ShaderBuffer += "VS_OUTPUT VS(VS_INPUT_PNABTI input1, uint instanceID : SV_InstanceID)\n";
		ShaderBuffer += "{\n";
		ShaderBuffer += "	VS_VERTEX Vertex = (VS_VERTEX)0;\n";
		ShaderBuffer += "	Vertex.Pos = input.Pos;\n";
		ShaderBuffer += "	Vertex.Normal = input.Normal; \n";
		ShaderBuffer += "	Vertex.Tangent = input.Tangent;\n";
		ShaderBuffer += "	Vertex.Binormal = input.Binormal;\n";
		ShaderBuffer += "	Vertex.TexCoord = input.TexCoord;\n";
		ShaderBuffer += "	Vertex.SupportNormalMap = true;\n";
		ShaderBuffer += "	Vertex.InstanceID = instanceID;\n";
		ShaderBuffer += "	Vertex.InstancePos = input.InstancePos;\n";
		ShaderBuffer += "	return ComputeVertex(Vertex);\n";
		ShaderBuffer += "}\n";
	}
	else
	{
		// Unsupported layout 
		LXString Layouts = L"Unsupported layout:";
		if (VSSignature.LayoutMask & LX_PRIMITIVE_INDICES) { Layouts += L" LX_PRIMITIVE_INDICES"; }
		if (VSSignature.LayoutMask & LX_PRIMITIVE_POSITIONS) { Layouts += L" LX_PRIMITIVE_POSITIONS"; }
		if (VSSignature.LayoutMask & LX_PRIMITIVE_NORMALS) { Layouts += L" LX_PRIMITIVE_NORMALS"; }
		if (VSSignature.LayoutMask & LX_PRIMITIVE_TANGENTS) { Layouts += L" LX_PRIMITIVE_TANGENTS"; }
		if (VSSignature.LayoutMask & LX_PRIMITIVE_TEXCOORDS) { Layouts += L" LX_PRIMITIVE_TEXCOORDS"; }
		if (VSSignature.LayoutMask & LX_PRIMITIVE_BINORMALS) { Layouts += L" LX_PRIMITIVE_BINORMALS"; }
		if (VSSignature.LayoutMask & LX_PRIMITIVE_INSTANCEPOSITIONS) { Layouts += L" LX_PRIMITIVE_INSTANCEPOSITIONS"; }
		LogE(LXShaderManager, Layouts.GetBuffer());
		CHK(0);
	}

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
	LXStringA ShaderBuffer = graphMaterialToHLSLConverer.GenerateCode(materialD3D11, renderPass);

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
		LXStringA texturesDecl = ListTexturesToHLSL(materialD3D11->GetTexturesPS());
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

LXStringA LXShaderFactory::ListTexturesToHLSL(const list<LXTextureD3D11*>& listTextures)
{
	//
	// Textures and TextureSamplers
	//

	LXStringA HLSLDeclaration;
	
	for (LXTextureD3D11* textureD3D11 : listTextures)
	{
		//if (!TextureSampler->GetTexture() || TextureSampler->AffectedShaders == 0)
		//{
		//	continue;
		//}

		/*
		// VertexShader
		if (TextureSampler->AffectedShaders & EShader::VertexShader)
		{
			LXStringA n = LXStringA::Number(TextureSampler->Slot);
			LXStringA HLSLTexture = "Texture2D texture" + n + " : register(vs, t" + n + ");\n";
			LXStringA HLSLSampler = "SamplerState sampler" + n + " : register(vs, s" + n + ");\n\n";
			HLSLTextureDeclarationVS += HLSLTexture;
			HLSLTextureDeclarationVS += HLSLSampler;
		}
		*/

		// PixelShader 
		//if (TextureSampler->AffectedShaders & EShader::PixelShader)
		{
			LXStringA n = LXStringA::Number(textureD3D11->Slot);
			HLSLDeclaration += "Texture2D texture" + n + " : register(ps, t" + n + ");\n";
			HLSLDeclaration += "SamplerState sampler" + n + " : register(ps, s" + n + ");\n\n";
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
