//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "LXShaderManager.h"
#include "LXConsoleManager.h"
#include "LXCore.h"
#include "LXDirectX11.h"
#include "LXFile.h"
#include "LXFileWatcher.h"
#include "LXInputElementDescD3D11Factory.h"
#include "LXLogger.h"
#include "LXMaterial.h"
#include "LXMaterialD3D11.h"
#include "LXPrimitiveD3D11.h"
#include "LXProject.h"
#include "LXRenderPass.h"
#include "LXSettings.h"
#include "LXShader.h"
#include "LXShaderD3D11.h"
#include "LXShaderFactory.h"
#include "LXShaderProgramD3D11.h"
#include "LXMemory.h" // --- Must be the last included ---

#define DEFAULT_SHADER L"Default.hlsl"
#define DRAWTOBACKBUFFER_SHADER L"DrawToBackBuffer.hlsl"

namespace
{
	LXConsoleCommandT<bool> CSet_EngineShaderDevMode(L"Engine.ini", L"Development", L"EngineShaderDevMode", L"false");
}

LXShaderManager::LXShaderManager()
{
	if (CSet_EngineShaderDevMode.GetValue())
	{
		_engineFileWatcher = make_unique<LXFileWatcher>(GetSettings().GetShadersFolder(), true);
		_engineFileWatcher->OnFileChanded([this](const wstring& filepath)
		{
			wchar_t buffer[MAX_PATH];
			wchar_t* filename = nullptr;
			GetFullPathNameW(filepath.c_str(), MAX_PATH, (LPWSTR)&buffer, (LPWSTR*)& filename);
			if (filename[0] != '_')
			{
				auto it = _monitoredShaderFiles.find(filename);
				if (it != _monitoredShaderFiles.end())
				{
					for (LXShaderD3D11* shader : it->second)
					{
						_shaderToRebuild.insert(shader);
					}
				}
			}
		});
	}
}

LXShaderManager::~LXShaderManager()
{
	DeleteUnusedShaders();

	LX_SAFE_DELETE(VSDrawToBackBuffer);
	LX_SAFE_DELETE(PSDrawToBackBuffer);
}

void LXShaderManager::CreateDefaultShaders()
{
	//
	// ScreenSpace shader used to draw a RTV to BackBuffer
	//
	
	VSDrawToBackBuffer = new LXShaderD3D11();
	PSDrawToBackBuffer = new LXShaderD3D11();

	const LXArrayInputElementDesc&  Layout = GetInputElementDescD3D11Factory().GetInputElement_PT();
	VSDrawToBackBuffer->CreateVertexShader(GetSettings().GetShadersFolder() + DRAWTOBACKBUFFER_SHADER, &Layout[0], (uint)Layout.size());
	PSDrawToBackBuffer->CreatePixelShader(GetSettings().GetShadersFolder() + DRAWTOBACKBUFFER_SHADER);
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

void LXShaderManager::DeleteUnusedShaders(bool keepErroneous)
{
	DeleteUnusedShaders(VertexShaders, keepErroneous);
	DeleteUnusedShaders(HullShaders, keepErroneous);
	DeleteUnusedShaders(DomainShaders, keepErroneous);
	DeleteUnusedShaders(GeometryShaders, keepErroneous);
	DeleteUnusedShaders(PixelShaders, keepErroneous);
}

template<typename M>
void LXShaderManager::DeleteUnusedShaders(M& mapShaders, bool keepErroneous)
{
	for (auto It = mapShaders.begin(); It != mapShaders.end();)
	{
		if (It->second.use_count() == 1 && !keepErroneous)
		{
			It = mapShaders.erase(It);
		}
		else
		{
			It++;
		}
	}
}

bool LXShaderManager::GetShaderSimple(LXShaderD3D11 * OutVS, LXShaderD3D11 * OutPS)
{
	return false;
}

bool LXShaderManager::GetShaders(ERenderPass RenderPass, const LXPrimitiveD3D11* InPrimitive, const LXMaterialD3D11* InMaterial, LXShaderProgramD3D11* OutShaderProgram)
{
	//
	// Vertex Shader
	//

	{
		LXVSSignature VSSignature;

		VSSignature.Material = InMaterial;
		VSSignature.Layout = &(*InPrimitive->Layout2)[0];
		VSSignature.LayoutElements = (uint)InPrimitive->Layout2->size();
		VSSignature.LayoutMask = InPrimitive->layoutMask;
		VSSignature.RenderPass = RenderPass;
		OutShaderProgram->VertexShader = FindOrCreate(VSSignature, VertexShaders);
		if (!OutShaderProgram->VertexShader || OutShaderProgram->VertexShader->GetState() != EShaderD3D11State::Ok)
			return false;
	}
	
	//
	// Hull Shader
	//

	if (InMaterial->DoComputeNormals())
	{
		LXHSSignature HSSignature = { 0 };
		OutShaderProgram->HullShader = FindOrCreate(HSSignature, HullShaders);
		if (!OutShaderProgram->HullShader || OutShaderProgram->HullShader->GetState() != EShaderD3D11State::Ok)
			return false;
	}
	
	//
	// Domain Shader
	//


	if (InMaterial->DoComputeNormals())
	{
		LXDSSignature DSSignature = { 0 };
		OutShaderProgram->DomainShader = FindOrCreate(DSSignature, DomainShaders);
		if (!OutShaderProgram->DomainShader || OutShaderProgram->DomainShader->GetState() != EShaderD3D11State::Ok)
			return false;
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
		PSSignature.Material = InMaterial;
		PSSignature.RenderPass = RenderPass;
		OutShaderProgram->PixelShader = FindOrCreate(PSSignature, PixelShaders);
		if (!OutShaderProgram->PixelShader || OutShaderProgram->PixelShader->GetState() != EShaderD3D11State::Ok)
			return false;
	}
	
	return true;
}

LXShaderD3D11* LXShaderManager::GetTextureShader(const LXString& ShaderFilename)
{
	CHK(0);
	return nullptr;
}

void LXShaderManager::AddMonitoredShaderFile(const wstring& filename, LXShaderD3D11* shaderD3D11)
{
	CHK(IsRenderThread());
	_monitoredShaderFiles[filename].push_back(shaderD3D11);
}

void LXShaderManager::Run()
{
	CHK(IsRenderThread());
	
	// Deletes unused shaders but keeps the erroneous,  
	// to avoid infinite 'rebuild' tentative without 'fix'.
	DeleteUnusedShaders(true);

	for (LXShaderD3D11* shader : _shaderToRebuild)
	{
		shader->Create();
	}
	_shaderToRebuild.clear();
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
	// Retrieve the shader filename
	LXFilepath ShaderFilePath = VSSignature.Material->GetMaterial()->GetFilepath();

	// Compute the shader filename and generate the it (EDITOR only)
	const LXString BaseName = ShaderFilePath.GetFilenameNoExt();
	LXFilepath GeneretedShaderFilePath = GetProject()->GetAssetFolder() + L"Shaders/" + LXShaderFactory::BuildShaderFilename(BaseName, VSSignature.RenderPass, VSSignature.LayoutMask, EShader::VertexShader);
		
	LXShaderFactory::GenerateVertexShader(GeneretedShaderFilePath, VSSignature.Material, VSSignature.RenderPass, VSSignature.LayoutMask);

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
	// Material 2.0 only the GBuffer shaders can be generated
	if (PSSignature.Material && (PSSignature.RenderPass == ERenderPass::GBuffer || PSSignature.RenderPass == ERenderPass::Transparency))
	{
		// Retrieve the shader filename
		LXFilepath ShaderFilePath = PSSignature.Material->GetMaterial()->GetFilepath();
		
		// Compute the shader filename and generate the it (EDITOR only)
		const LXString BaseName = ShaderFilePath.GetFilenameNoExt();
		LXFilepath GeneretedShaderFilePath = GetProject()->GetAssetFolder() + L"Shaders/" + LXShaderFactory::BuildShaderFilename(BaseName, PSSignature.RenderPass, -1, EShader::PixelShader);
		
		LXShaderFactory::GeneratePixelShader(GeneretedShaderFilePath, PSSignature.Material, PSSignature.RenderPass);

		return Shader->CreatePixelShader(GeneretedShaderFilePath);
	}

	switch (PSSignature.RenderPass)
	{
	case ERenderPass::Depth:
	case ERenderPass::Shadow:
		return Shader->CreatePixelShader(GetSettings().GetShadersFolder() + DEFAULT_SHADER, "PS_DEPTHONLY");
		break;
	case ERenderPass::GBuffer:
	case ERenderPass::Transparency:
		return Shader->CreatePixelShader(GetSettings().GetShadersFolder() + DEFAULT_SHADER);
		break;
	default: CHK(0); return false; break;
	}
}
