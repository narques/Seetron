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
class LXMaterialBase;
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
	void Update(const LXMaterialBase* Material);

	const LXMaterialBase* GetMaterial() const { return Material; }
	bool DoComputeNormals() const { return ComputeNormals; }
	bool IsTransparent() const { return Transparent; }

	const LXConstantBuffer& GetConstantBufferPS() const { return ConstantBufferPS; }
	const LXConstantBuffer& GetConstantBufferVS() const { return ConstantBufferVS; }
	const std::list<LXTexture*>& GetTexturesVS() const { return ListVSTextures; }
	const std::list<LXTexture*>& GetTexturesPS() const { return ListPSTextures; }

	// Helpers
	static std::shared_ptr<LXMaterialD3D11> CreateFromMaterial(const LXMaterialBase* material);
			
private:

	bool Create(const LXMaterialBase*);
	void UpdateConstantBufferDataValues();

private:

	// List of texture to bind to the corresponding shader
	std::list<LXTexture*> ListVSTextures;
	std::list<LXTexture*> ListPSTextures;

	// States
	uint TwoSided : 1;

	// VertexShader option
	uint ComputeNormals : 1;
	uint Transparent : 1;

	// Ref.
	const LXMaterialBase* Material = nullptr;

	bool _bValid = false;

	// VS Constant Buffer 
	LXConstantBuffer ConstantBufferVS;
	LXConstantBufferD3D11* CBMaterialParemetersVS = nullptr;

	// PS Constant Buffer 
	LXConstantBuffer ConstantBufferPS;
	LXConstantBufferD3D11* CBMaterialParemetersPS = nullptr;

};

