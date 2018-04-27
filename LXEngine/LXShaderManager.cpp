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
#include "LXShaderFactory.h"
#include "LXMemory.h" // --- Must be the last included ---

// New feature: All shaders are generated, the user shader files must adopt a format
// The Generated file will include the user file.
#define LX_GENERATED_SHADER 1

#define DEFAULT_SHADER L"Default.hlsl"
#define DRAWTOBACKBUFFER_SHADER L"DrawToBackBuffer.hlsl"


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
}


LXShaderManager::~LXShaderManager()
{
	DeleteShaders();

	LX_SAFE_DELETE(VSDrawToBackBuffer);
	LX_SAFE_DELETE(PSDrawToBackBuffer);
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

bool LXShaderManager::CreateShader(const LXVSSignature& VSSignature, LXShaderD3D11* Shader)
{
	if (VSSignature.displacement)
		Shader->AddMacro("DISPLACEMENT", "1");

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

	// Compute the shader filename and generate the it (EDITOR only)
	const LXString BaseName = ShaderFilePath.GetFilenameNoExt();
	LXFilepath GeneretedShaderFilePath = ShaderFilePath.GetFilepath() + LXShaderFactory::BuildShaderFilename(BaseName, VSSignature.RenderPass, VSSignature.LayoutMask, EShader::VertexShader);
	LXShaderFactory::GenerateVertexShader(GeneretedShaderFilePath, ShaderFilePath.GetFilename(), VSSignature);

	return Shader->CreateVertexShader(GeneretedShaderFilePath, VSSignature.Layout, VSSignature.LayoutElements);
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
