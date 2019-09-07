//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXString.h"
#include "LXConstantBuffer.h"
#include "LXRenderPassType.h"

class LXRenderCommandList;
class LXMaterial;
class LXTexture;
class LXMaterialNodeTextureSampler;
class LXConstantBufferD3D11;
class LXShaderProgramD3D11;

class LXMaterialD3D11 : public LXObject
{

public:

	LXMaterialD3D11();
	~LXMaterialD3D11();

	void Release();
	void Render(ERenderPass RenderPass, LXRenderCommandList* RCL) const;
	void Update(const LXMaterial* Material);

	const LXMaterial* GetMaterial() const { return Material; }
	bool DoComputeNormals() const { return ComputeNormals; }
	bool IsTransparent() const { return Transparent; }

	const LXConstantBuffer& GetConstantBufferPS() const { return ConstantBufferPS; }
	const list<LXTexture*>& GetTexturesVS() const { return ListVSTextures; }
	const list<LXTexture*>& GetTexturesPS() const { return ListPSTextures; }

	// Helpers
	static LXMaterialD3D11* CreateFromMaterial(const LXMaterial* material);
			
private:

	bool Create(const LXMaterial*);
	bool CreateShaders();
	void UpdateConstantBufferDataValues();

public:

	mutable int HLSLTextureIndex = 0;
	
private:

	// List of texture to bind to the corresponding shader
	list<LXTexture*> ListVSTextures;
	list<LXTexture*> ListPSTextures;

	// States
	uint TwoSided : 1;

	// VertexShader option
	uint ComputeNormals : 1;
	uint Transparent : 1;

	// Ref.
	const LXMaterial* Material = nullptr;

	bool _bValid = false;

	// VS Constant Buffer 
	LXConstantBuffer ConstantBufferVS;
	LXConstantBufferD3D11* CBMaterialParemetersVS = nullptr;

	// PS Constant Buffer 
	LXConstantBuffer ConstantBufferPS;
	LXConstantBufferD3D11* CBMaterialParemetersPS = nullptr;

};

