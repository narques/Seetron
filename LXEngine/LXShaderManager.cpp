//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "LXShaderManager.h"
#include "LXShaderD3D11.h"
#include "LXMaterialD3D11.h"
#include "LXMaterial.h"
#include "LXShader.h"
#include "LXSettings.h"
#include "LXDirectX11.h"
#include "LXPrimitiveD3D11.h"
#include "LXFile.h"
#include "LXLogger.h"
#include "LXInputElementDescD3D11Factory.h"
#include "LXRenderPass.h"
#include "LXShaderProgramD3D11.h"
#include "LXMemory.h" // --- Must be the last included ---

// New feature: All shaders are generated, the user shader files must adopt a format
// The Generated file will include the user file.
#define LX_GENERATED_SHADER 1

#define DEFAULT_SHADER L"Default.hlsl"
#define DRAWTOBACKBUFFER_SHADER L"DrawToBackBuffer.hlsl"
#define LIGHTING_SHADER L"Lighting.hlsl"


LXShaderManager::LXShaderManager()
{
	//
	// ScreenSpace shader used to draw a RTV to BackBuffer
	//
	{
		VSDrawToBackBuffer = new LXShaderD3D11();
		PSDrawToBackBuffer = new LXShaderD3D11();

		const LXArrayInputElementDesc&  Layout = GetInputElementDescD3D11Factory().GetInputElement_PT();
		VSDrawToBackBuffer->CreateVertexShader(GetSettings().GetShadersFolder() + DRAWTOBACKBUFFER_SHADER, &Layout[0], (uint)Layout.size());
		PSDrawToBackBuffer->CreatePixelShader(GetSettings().GetShadersFolder() + DRAWTOBACKBUFFER_SHADER);
	}

	//
	// Lighting
	//

	{
		VSLighting = new LXShaderD3D11();
		PSLighting = new LXShaderD3D11();
		CreateLightingVSShader(VSLighting);
		CreateLightingPSShader(PSLighting);
	}

	{
		//VSToneMapping = new LXShaderD3D11();
		//PSToneMapping = new LXShaderD3D11();
		//CreateToneMappingVSShader(VSToneMapping);
		//CreateToneMappingPSShader(PSToneMapping);
	}
}


LXShaderManager::~LXShaderManager()
{
	DeleteShaders();

	LX_SAFE_DELETE(VSDrawToBackBuffer);
	LX_SAFE_DELETE(PSDrawToBackBuffer);
	LX_SAFE_DELETE(VSLighting);
	LX_SAFE_DELETE(PSLighting);
	//LX_SAFE_DELETE(VSToneMapping);
	//LX_SAFE_DELETE(PSToneMapping);
}

void LXShaderManager::RebuildShaders()
{
	for (auto It = VertexShaders.begin(); It != VertexShaders.end(); It++)
	{
		LXShaderD3D11* Shader = It->second.get();
		CreateShader(It->first, Shader);
	}

	for (auto It = HullShaders.begin(); It != HullShaders.end(); It++)
	{
		LXShaderD3D11* Shader = It->second.get();
		CreateShader(It->first, Shader);
	}

	for (auto It = DomainShaders.begin(); It != DomainShaders.end(); It++)
	{
		LXShaderD3D11* Shader = It->second.get();
		CreateShader(It->first, Shader);
	}

	for (auto It = GeometryShaders.begin(); It != GeometryShaders.end(); It++)
	{
		LXShaderD3D11* Shader = It->second.get();
		CreateShader(It->first, Shader);
	}

	for (auto It = PixelShaders.begin(); It != PixelShaders.end(); It++)
	{
		LXShaderD3D11* Shader = It->second.get();
		CreateShader(It->first, Shader);
	}

	CreateLightingVSShader(VSLighting);
	CreateLightingPSShader(PSLighting);

	//CreateToneMappingVSShader(VSToneMapping);
	//CreateToneMappingPSShader(PSToneMapping);
}

void LXShaderManager::DeleteShaders()
{
	for (auto It = VertexShaders.begin(); It != VertexShaders.end(); It++)
	{
		CHK(It->second.use_count() == 1);
		//delete It->second;
	}

	for (auto It = HullShaders.begin(); It != HullShaders.end(); It++)
	{
		CHK(It->second.use_count() == 1);
		//delete It->second;
	}

	for (auto It = DomainShaders.begin(); It != DomainShaders.end(); It++)
	{
		CHK(It->second.use_count() == 1);
		//delete It->second;
	}

	for (auto It = GeometryShaders.begin(); It != GeometryShaders.end(); It++)
	{
		CHK(It->second.use_count() == 1);
		//delete It->second;
	}

	for (auto It = PixelShaders.begin(); It != PixelShaders.end(); It++)
	{
		CHK(It->second.use_count() == 1);
		//delete It->second;
	}
}

bool LXShaderManager::GetShaderSimple(LXShaderD3D11 * OutVS, LXShaderD3D11 * OutPS)
{
	return false;
}

bool LXShaderManager::GetShaders(ERenderPass RenderPass, const LXPrimitiveD3D11* InPrimitive, LXMaterialD3D11* InMaterial, LXShaderProgramD3D11* OutShaderProgram)
{
	bool bResult = true;

	//
	// Vertex Shader
	//

	{
		LXVSSignature VSSignature;
		VSSignature.Shader = InMaterial->GetMaterial()->VertexShader;
		VSSignature.Layout = &(*InPrimitive->Layout2)[0];
		VSSignature.LayoutElements = (uint)InPrimitive->Layout2->size();
		VSSignature.LayoutMask = InPrimitive->layoutMask;
		VSSignature.displacement = InMaterial->HasDisplacement() ? true : false;
		VSSignature.RenderPass = RenderPass;
		OutShaderProgram->VertexShader = FindOrCreate(VSSignature, VertexShaders);
		if (!OutShaderProgram->VertexShader || OutShaderProgram->VertexShader->GetState() != EShaderD3D11State::Ok)
			bResult = false;
	}
	
	//
	// Hull Shader
	//

	if (InMaterial->DoComputeNormals())
	{
		LXHSSignature HSSignature = { 0 };
		OutShaderProgram->HullShader = FindOrCreate(HSSignature, HullShaders);
		if (!OutShaderProgram->HullShader || OutShaderProgram->HullShader->GetState() != EShaderD3D11State::Ok)
			bResult = false;
	}
	
	//
	// Domain Shader
	//


	if (InMaterial->DoComputeNormals())
	{
		LXDSSignature DSSignature = { 0 };
		OutShaderProgram->DomainShader = FindOrCreate(DSSignature, DomainShaders);
		if (!OutShaderProgram->DomainShader || OutShaderProgram->DomainShader->GetState() != EShaderD3D11State::Ok)
			bResult = false;
	}

	//
	// Geometry Shader
	//

	LXGSSignature GSSignature = { 0 };

	//
	// Pixel Shader
	//

	{
		LXPSSignature PSSignature;
		PSSignature.Shader = InMaterial->GetMaterial()->PixelShader;
		PSSignature.RenderPass = RenderPass;
		OutShaderProgram->PixelShader = FindOrCreate(PSSignature, PixelShaders);
		if (!OutShaderProgram->PixelShader || OutShaderProgram->PixelShader->GetState() != EShaderD3D11State::Ok)
			bResult = false;
	}
	
	return bResult;
}

LXShaderD3D11* LXShaderManager::GetTextureShader(const LXString& ShaderFilename)
{
	CHK(0);
	return nullptr;
}

void LXShaderManager::Run()
{
	CHK(IsRenderThread())

	for (auto It = VertexShaders.begin(); It != VertexShaders.end(); It++)
	{
		if (It->second.use_count() == 1)
		{
			VertexShaders.erase(It);
		}
	}

	for (auto It = HullShaders.begin(); It != HullShaders.end(); It++)
	{
		if (It->second.use_count() == 1)
		{
			HullShaders.erase(It);
		}
	}
	
	for (auto It = DomainShaders.begin(); It != DomainShaders.end(); It++)
	{
		if (It->second.use_count() == 1)
		{
			DomainShaders.erase(It);
		}
	}

	for (auto It = GeometryShaders.begin(); It != GeometryShaders.end(); It++)
	{
		if (It->second.use_count() == 1)
		{
			GeometryShaders.erase(It);
		}
	}
	
	for (auto It = PixelShaders.begin(); It != PixelShaders.end(); It++)
	{
		if (It->second.use_count() == 1)
		{
			PixelShaders.erase(It);
		}
	}
}

template<typename T, typename M>
shared_ptr<LXShaderD3D11> LXShaderManager::FindOrCreate(T& Signature, M& Shaders)
{
	auto it = Shaders.find(Signature);
	if (it != Shaders.end())
	{
		return it->second;
	}
	else
	{
		shared_ptr<LXShaderD3D11> Shader = make_shared<LXShaderD3D11>();
		CreateShader(Signature, Shader.get());
		Shaders[Signature] = Shader;
		return Shader;
	}
}

// void LXShaderManager::CreateShader(const LXVSSignature& VSSignature, LXShaderD3D11* Shader)
// {
// 	D3D11_INPUT_ELEMENT_DESC* Layout2 = LXPrimitiveD3D11::Layout_PNABT;
// 	UINT numElements = sizeof(LXPrimitiveD3D11::Layout_PNABT) / sizeof(LXPrimitiveD3D11::Layout_PNABT[0]);
// 
// 	LXString ShaderFilename = DEFAULT_SHADER;
// 	if (VSSignature.Shader != "")
// 	{
// 		ShaderFilename = VSSignature.Shader;
// 		Layout2 = LXPrimitiveD3D11::Layout_PT;
// 		numElements = sizeof(LXPrimitiveD3D11::Layout_PT) / sizeof(LXPrimitiveD3D11::Layout_PT[0]);
// 	}
// 
// 	if (VSSignature.displacement)
// 		Shader->AddMacro("DISPLACEMENT", "1");
// 
// 	Shader->CreateVertexShader(GetSettings().GetShadersFolder() + ShaderFilename, Layout2, numElements);
// }

bool LXShaderManager::CreateShader(const LXVSSignature& VSSignature, LXShaderD3D11* Shader)
{
	if (VSSignature.displacement)
		Shader->AddMacro("DISPLACEMENT", "1");

#if LX_GENERATED_SHADER

	// Retrieve the shader filename
	LXFilepath ShaderFilePath;
	if (VSSignature.Shader)
	{
		ShaderFilePath = VSSignature.Shader->GetFilepath();
	}
	else
	{
		ShaderFilePath = GetSettings().GetShadersFolder() + DEFAULT_SHADER;
	}

	// Compute the Generated shader filename

	const LXString BaseName = ShaderFilePath.GetFilenameNoExt();
	LXFilepath GeneretedShaderFilePath = ShaderFilePath.GetFilepath() + BuildShaderFilename(BaseName, VSSignature.RenderPass, VSSignature.LayoutMask, EShader::VertexShader);

	// If the generated file already exists, we regenerate it only if the user file is more recent.
// 	if (GeneretedShaderFilePath.IsFileExist())
// 	{
// 		VRF(LXFileUtility::GetFileLastWriteTime(GeneretedShaderFilePath));
// 	}
// 	else
	{
		GenerateVertexShader(GeneretedShaderFilePath, ShaderFilePath.GetFilename(), VSSignature);
	}

	return Shader->CreateVertexShader(GeneretedShaderFilePath, VSSignature.Layout, VSSignature.LayoutElements);
		
#else



	// For now, only the GBuffer shaders can be overridden by user.
	if (VSSignature.Shader && VSSignature.RenderPass == ERenderPass::GBuffer)
	{
		return Shader->CreateVertexShader(VSSignature.Shader->GetFilepath(), VSSignature.Layout, VSSignature.LayoutElements);
	}
	else
	{
		LXString ShaderFilename = GenerateDefaultVShader(VSSignature);
		return Shader->CreateVertexShader(GetSettings().GetShadersFolder() + ShaderFilename, VSSignature.Layout, VSSignature.LayoutElements);
	}

#endif
}

bool LXShaderManager::CreateShader(const LXHSSignature& HSSignature, LXShaderD3D11* Shader)
{
	return Shader->CreateHullShader(GetSettings().GetShadersFolder() + DEFAULT_SHADER);
}

bool LXShaderManager::CreateShader(const LXDSSignature& DSSignature, LXShaderD3D11* Shader)
{
	return Shader->CreateDomainShader(GetSettings().GetShadersFolder() + DEFAULT_SHADER);
}

bool LXShaderManager::CreateShader(const LXGSSignature& GSSignature, LXShaderD3D11* Shader)
{
	return Shader->CreateGeometryShader(GetSettings().GetShadersFolder() + DEFAULT_SHADER);
}

bool LXShaderManager::CreateShader(const LXPSSignature& PSSignature, LXShaderD3D11* Shader)
{
	// For now, only the GBuffer shaders can be overridden by user.
	if (PSSignature.Shader && PSSignature.RenderPass == ERenderPass::GBuffer)
	{
		return Shader->CreatePixelShader(PSSignature.Shader->GetFilepath());
	}
	else
	{
		switch (PSSignature.RenderPass)
		{
		case ERenderPass::Depth:
		case ERenderPass::Shadow:
			return Shader->CreatePixelShader(GetSettings().GetShadersFolder() + DEFAULT_SHADER, "PS_DEPTHONLY");
			break;
		case ERenderPass::GBuffer:
			return Shader->CreatePixelShader(GetSettings().GetShadersFolder() + DEFAULT_SHADER);
			break;

		default: CHK(0); return false; break;
		}
	}
}

void LXShaderManager::CreateLightingVSShader(LXShaderD3D11* Shader)
{
	const LXArrayInputElementDesc& Layout = GetInputElementDescD3D11Factory().GetInputElement_PT();
	Shader->CreateVertexShader(GetSettings().GetShadersFolder() + LIGHTING_SHADER, &Layout[0], (uint)Layout.size());
}

void LXShaderManager::CreateLightingPSShader(LXShaderD3D11* Shader)
{
	Shader->CreatePixelShader(GetSettings().GetShadersFolder() + LIGHTING_SHADER);
}

void LXShaderManager::GenerateVertexShader(const LXFilepath& Filename, const LXString& FileToInclude, const LXVSSignature& VSSignature)
{
	LXStringA ShaderBuffer;

	// Common
	ShaderBuffer += "// *** Generated file ***\n";
	ShaderBuffer += "#include \"" + LXStringA(&*FileToInclude.GetBufferA().begin()) + "\"\n";
	ShaderBuffer += "\n";

	if (VSSignature.LayoutMask == (int)TRUCMACHIN::P)
	{
		ShaderBuffer += "VS_OUTPUT VS(VS_INPUT_P input)\n";
		ShaderBuffer += "{\n";
		ShaderBuffer += "	VS_VERTEX Vertex = (VS_VERTEX)0;\n";
		ShaderBuffer += "	Vertex.Pos = input.Pos;\n";
		ShaderBuffer += "	return ComputeVertex(Vertex);\n";
		ShaderBuffer += "}\n";

	}
	else if (VSSignature.LayoutMask == (int)TRUCMACHIN::PT)
	{
		CHK(0);
	}
	else if (VSSignature.LayoutMask == (int)TRUCMACHIN::PN)
	{
		ShaderBuffer += "VS_OUTPUT VS(VS_INPUT_PN input)\n";
		ShaderBuffer += "{\n";
		ShaderBuffer += "	VS_VERTEX Vertex = (VS_VERTEX)0;\n";
		ShaderBuffer += "	Vertex.Pos = input.Pos;\n";
		ShaderBuffer += "	Vertex.Normal = input.Normal; \n";
		ShaderBuffer += "	return ComputeVertex(Vertex);\n";
		ShaderBuffer += "}\n";
	}
	else if (VSSignature.LayoutMask == (int)TRUCMACHIN::PNT)
	{
		ShaderBuffer += "VS_OUTPUT VS(VS_INPUT_PNT input)\n";
		ShaderBuffer += "{\n";
		ShaderBuffer += "	VS_VERTEX Vertex = (VS_VERTEX)0;\n";
		ShaderBuffer += "	Vertex.Pos = input.Pos;\n";
		ShaderBuffer += "	Vertex.Normal = input.Normal; \n";
		ShaderBuffer += "	Vertex.TexCoord = input.TexCoord;\n";
		ShaderBuffer += "	return ComputeVertex(Vertex);\n";
		ShaderBuffer += "}\n";
	}
	else if (VSSignature.LayoutMask == (int)TRUCMACHIN::PNABT)
	{
		ShaderBuffer += "VS_OUTPUT VS(VS_INPUT_PNABT input)\n";
		ShaderBuffer += "{\n";
		ShaderBuffer += "	VS_VERTEX Vertex = (VS_VERTEX)0;\n";
		ShaderBuffer += "	Vertex.Pos = input.Pos;\n";
		ShaderBuffer += "	Vertex.Normal = input.Normal; \n";
		ShaderBuffer += "	Vertex.Tangent = input.Tangent;\n";
		ShaderBuffer += "	Vertex.Binormal = input.Binormal;\n";
		ShaderBuffer += "	Vertex.TexCoord = input.TexCoord;\n";
		ShaderBuffer += "	return ComputeVertex(Vertex);\n";
		ShaderBuffer += "}\n";
	}
	else if (VSSignature.LayoutMask == (int)TRUCMACHIN::PNABTI)
	{
		ShaderBuffer += "VS_OUTPUT VS(VS_INPUT_PNABTI input1, uint instanceID : SV_InstanceID)\n";
		ShaderBuffer += "{\n";
		ShaderBuffer += "	VS_VERTEX Vertex = (VS_VERTEX)0;\n";
		ShaderBuffer += "	Vertex.Pos = input.Pos;\n";
		ShaderBuffer += "	Vertex.Normal = input.Normal; \n";
		ShaderBuffer += "	Vertex.Tangent = input.Tangent;\n";
		ShaderBuffer += "	Vertex.Binormal = input.Binormal;\n";
		ShaderBuffer += "	Vertex.TexCoord = input.TexCoord;\n";
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


LXString LXShaderManager::GenerateDefaultVShader(const LXVSSignature& VSSignature)
{
	LXStringA ShaderBuffer;
	
	// Common
	ShaderBuffer += "// *** Generated file ***\n";
	ShaderBuffer += "#include \"common.hlsl\"\n";
	ShaderBuffer += "\n";

	LXString ShaderFilename;

	if (VSSignature.LayoutMask == (int)TRUCMACHIN::P)
	{
		ShaderFilename = "_Default_P_VS.hlsl";
		ShaderBuffer += "VS_OUTPUT VS(VS_INPUT_P input)\n";
		ShaderBuffer += "{\n";
		ShaderBuffer += "	return VS_P(input);\n";
		ShaderBuffer += "}\n";

	}
	else if (VSSignature.LayoutMask == (int)TRUCMACHIN::PT)
	{
		ShaderFilename = "_Default_PT_VS.hlsl";
		CHK(0);
	}
	else if (VSSignature.LayoutMask == (int)TRUCMACHIN::PN)
	{
		ShaderFilename = "_Default_PN_VS.hlsl";
		ShaderBuffer += "VS_OUTPUT VS(VS_INPUT_PN input)\n";
		ShaderBuffer += "{\n";
		ShaderBuffer += "	return VS_PN(input);\n";
		ShaderBuffer += "}\n";
	}
	else if (VSSignature.LayoutMask == (int)TRUCMACHIN::PNT)
	{
		ShaderFilename = "_Default_PNT_VS.hlsl";
		ShaderBuffer += "VS_OUTPUT VS(VS_INPUT_PNT input)\n";
		ShaderBuffer += "{\n";
		ShaderBuffer += "	return VS_PNT(input);\n";
		ShaderBuffer += "}\n";
	}
	else if (VSSignature.LayoutMask == (int)TRUCMACHIN::PNABT)
	{
		ShaderFilename = "_Default_PNABT_VS.hlsl";
		ShaderBuffer += "VS_OUTPUT VS(VS_INPUT_PNABT input)\n";
		ShaderBuffer += "{\n";
		ShaderBuffer += "	return VS_PNABT(input);\n";
		ShaderBuffer += "}\n";
	}
	else if (VSSignature.LayoutMask == (int)TRUCMACHIN::PNABTI)
	{
		ShaderFilename = "_Default_PNABTI_VS.hlsl";
		ShaderBuffer += "VS_OUTPUT VS(VS_INPUT_PNABTI input1, uint instanceID : SV_InstanceID)\n";
		ShaderBuffer += "{\n";
		ShaderBuffer += "	return VS_PNABTI(input1, instanceID);\n";
		ShaderBuffer += "}\n";
	}
	else 
	{
		// Unsupported layout 
		LXString Layouts = L"Unsupported layout:";
		if (VSSignature.LayoutMask & LX_PRIMITIVE_INDICES) 	{ Layouts += L" LX_PRIMITIVE_INDICES"; }
		if (VSSignature.LayoutMask & LX_PRIMITIVE_POSITIONS) 	{ Layouts += L" LX_PRIMITIVE_POSITIONS"; }
		if (VSSignature.LayoutMask & LX_PRIMITIVE_NORMALS) 	{ Layouts += L" LX_PRIMITIVE_NORMALS"; }
		if (VSSignature.LayoutMask & LX_PRIMITIVE_TANGENTS) { Layouts += L" LX_PRIMITIVE_TANGENTS"; 	}
		if (VSSignature.LayoutMask & LX_PRIMITIVE_TEXCOORDS) { Layouts += L" LX_PRIMITIVE_TEXCOORDS"; 	}
		if (VSSignature.LayoutMask & LX_PRIMITIVE_BINORMALS) { Layouts += L" LX_PRIMITIVE_BINORMALS"; 	}
		if (VSSignature.LayoutMask & LX_PRIMITIVE_INSTANCEPOSITIONS) { Layouts += L" LX_PRIMITIVE_INSTANCEPOSITIONS"; }
		LogE(LXShaderManager, Layouts.GetBuffer());
		CHK(0);
		return L"";
	}
	
	LXString ShaderFilepath = GetSettings().GetShadersFolder() + ShaderFilename;
	LXPlatform::DeleteFile(ShaderFilepath);
		
	LXFile File;
	if (File.Open(ShaderFilepath, "wt") == true)
	{
		File.Write(ShaderBuffer.GetBuffer(), ShaderBuffer.size());
		File.Close();
	}
	else
		LogE(ShaderManager, L"Unable to open shader file: %s", ShaderFilepath.GetBuffer());

	return ShaderFilename;
}

// FILENAME :  _MATERIALNAME_CONTEXT_LAYOUT_DOMAIN.hlsl
// _M_RedMetal_Shadow_P_VS.hlsl

LXString LXShaderManager::GetRenderPassName(ERenderPass RenderPass)
{
	switch (RenderPass)
	{
	case ERenderPass::Depth: return L"_Depth"; break;
	case ERenderPass::Shadow: return L"_Shadow"; break;
	case ERenderPass::GBuffer: return L"_GBuffer"; break;
	default: CHK(0); return L""; break;
	}
}

LXString LXShaderManager::GetShaderName(EShader Shader)
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

LXString LXShaderManager::GetLayoutName(int LayoutMask)
{
	switch (LayoutMask)
	{
	case (int)TRUCMACHIN::P: return L"_P"; break;
	case (int)TRUCMACHIN::PT: return L"_PT"; break;
	case (int)TRUCMACHIN::PN: return L"_PN"; break;
	case (int)TRUCMACHIN::PNT: return L"_PNT"; break;
	case (int)TRUCMACHIN::PNABT: return L"_PNABT"; break;
	case (int)TRUCMACHIN::PNABTI: return L"_PNABTI"; break;
	default: CHK(0); return L""; break;
	}
}

LXString LXShaderManager::BuildShaderFilename(const LXString& BaseName, ERenderPass RenderPass, int LayoutMask, EShader Shader)
{
	LXString Filename = L"_";
	Filename += BaseName;
	Filename += GetRenderPassName(RenderPass);
	Filename += GetLayoutName(LayoutMask);
	Filename += GetShaderName(Shader);
	Filename += L".hlsl";
	return Filename;
}

#if 0

void GenerateVertexShader_RenderPassShadow(int LayoutMask)
{
	LXStringA ShaderBuffer;

	// Common
	ShaderBuffer += "// *** Generated file ***\n";
	ShaderBuffer += "#include \"common.hlsl\"\n";
	ShaderBuffer += "\n";

	LXString ShaderFilename;

	if (LayoutMask == (int)TRUCMACHIN::P)
	{
		ShaderFilename = "_Default_P_VS.hlsl";
		ShaderBuffer += "VS_OUTPUT VS(VS_INPUT_P input)\n";
		ShaderBuffer += "{\n";
		ShaderBuffer += "	return VS_P(input);\n";
		ShaderBuffer += "}\n";

	}
	else if (LayoutMask == (int)TRUCMACHIN::PT)
	{
		ShaderFilename = "_Default_PT_VS.hlsl";
		CHK(0);
	}
	else if (LayoutMask == (int)TRUCMACHIN::PN)
	{
		ShaderFilename = "_Default_PN_VS.hlsl";
		ShaderBuffer += "VS_OUTPUT VS(VS_INPUT_PN input)\n";
		ShaderBuffer += "{\n";
		ShaderBuffer += "	return VS_PN(input);\n";
		ShaderBuffer += "}\n";
	}
	else if (LayoutMask == (int)TRUCMACHIN::PNT)
	{
		ShaderFilename = "_Default_PNT_VS.hlsl";
		ShaderBuffer += "VS_OUTPUT VS(VS_INPUT_PNT input)\n";
		ShaderBuffer += "{\n";
		ShaderBuffer += "	return VS_PNT(input);\n";
		ShaderBuffer += "}\n";
	}
	else if (LayoutMask == (int)TRUCMACHIN::PNABT)
	{
		ShaderFilename = "_Default_PNABT_VS.hlsl";
		ShaderBuffer += "VS_OUTPUT VS(VS_INPUT_PNABT input)\n";
		ShaderBuffer += "{\n";
		ShaderBuffer += "	return VS_PNABT(input);\n";
		ShaderBuffer += "}\n";
	}
	else if (LayoutMask == (int)TRUCMACHIN::PNABTI)
	{
		ShaderFilename = "_Default_PNABTI_VS.hlsl";
		ShaderBuffer += "VS_OUTPUT VS(VS_INPUT_PNABTI input1, uint instanceID : SV_InstanceID)\n";
		ShaderBuffer += "{\n";
		ShaderBuffer += "	return VS_PNABTI(input1, instanceID);\n";
		ShaderBuffer += "}\n";
	}
	else
	{
		// Unsupported layout 
		LXString Layouts = L"Unsupported layout:";
		if (LayoutMask & LX_PRIMITIVE_INDICES) { Layouts += L" LX_PRIMITIVE_INDICES"; }
		if (LayoutMask & LX_PRIMITIVE_POSITIONS) { Layouts += L" LX_PRIMITIVE_POSITIONS"; }
		if (LayoutMask & LX_PRIMITIVE_NORMALS) { Layouts += L" LX_PRIMITIVE_NORMALS"; }
		if (LayoutMask & LX_PRIMITIVE_TANGENTS) { Layouts += L" LX_PRIMITIVE_TANGENTS"; }
		if (LayoutMask & LX_PRIMITIVE_TEXCOORDS) { Layouts += L" LX_PRIMITIVE_TEXCOORDS"; }
		if (LayoutMask & LX_PRIMITIVE_BINORMALS) { Layouts += L" LX_PRIMITIVE_BINORMALS"; }
		if (LayoutMask & LX_PRIMITIVE_INSTANCEPOSITIONS) { Layouts += L" LX_PRIMITIVE_INSTANCEPOSITIONS"; }
		LogE(LXShaderManager, Layouts.GetBuffer());
		CHK(0);
		return L"";
	}

	LXString ShaderFilepath = GetSettings().GetShadersFolder() + ShaderFilename;
	LXPlatform::DeleteFile(ShaderFilepath);

	LXFile File;
	if (File.Open(ShaderFilepath, "wt") == true)
	{
		File.Write(ShaderBuffer.GetBuffer(), ShaderBuffer.size());
		File.Close();
	}
	else
		LogE(ShaderManager, L"Unable to open shader file: %s", ShaderFilepath.GetBuffer());

	return ShaderFilename;
}

void GeneratePixelShader(ERenderPass RenderPass, int LayoutMask)
{
	switch (RenderPass)
	{
	//case ERenderPass::Depth:GeneratePixelShader_RenderPassDepth(LayoutMask); break;
	case ERenderPass::Shadow: GeneratePixelShader_RenderPassShadow(LayoutMask); break;
	//case ERenderPass::GBuffer:GeneratePixelShader_RenderPassGBuffer(LayoutMask); break;
	default:CHK(0); break;
	}
	
}

#endif

void GenerateShader()
{

}
