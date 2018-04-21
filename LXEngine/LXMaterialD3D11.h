//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXString.h"
#include "LXConstantBuffer.h"
#include "LXRenderPass.h"

class LXRenderCommandList;
class LXMaterial;
class LXTextureD3D11;
class LXMaterialNodeTextureSampler;
class LXConstantBufferD3D11;
class LXShaderProgramD3D11;

class LXMaterialD3D11 : public LXObject
{

public:

	LXMaterialD3D11(const LXMaterial*);
	~LXMaterialD3D11();

	//void Invalidate();
	void Render(ERenderPass RenderPass, LXRenderCommandList* RCL);
	void Update(const LXMaterial* Material);

	const LXMaterial* GetMaterial() const { return Material; }
	bool DoComputeNormals() const { return ComputeNormals; }
	bool HasDisplacement() const { return Displacement; }
	bool IsTransparent() const { return Transparent; }

	const LXConstantBuffer& GetConstantBufferPS() const { return ConstantBufferPS; }
			
private:

	void Release();
	bool Create(const LXMaterial*);
	LXTextureD3D11* CreateTexture(LXMaterialNodeTextureSampler* TextureSampler);
	void UpdateConstantBufferDataValues();


private:

	// List of texture to bind to the corresponding shader
	list<LXTextureD3D11*> ListVSTextures;
	list<LXTextureD3D11*> ListPSTextures;

	// States
	uint TwoSided : 1;

	// VertexShader option
	uint ComputeNormals : 1;
	uint Displacement : 1;
	uint Transparent : 1;

	//ShaderProgram
	LXShaderProgramD3D11* ShaderProgram = nullptr;

	// Ref.
	const LXMaterial* Material = nullptr;

	bool _bValid = false;

	// VS Constant Buffer 
	//LXConstantBuffer ConstantBufferVS;
	//LXConstantBufferD3D11* CBMaterialParemetersVS = nullptr;

	// PS Constant Buffer 
	LXConstantBuffer ConstantBufferPS;
	LXConstantBufferD3D11* CBMaterialParemetersPS = nullptr;

};

