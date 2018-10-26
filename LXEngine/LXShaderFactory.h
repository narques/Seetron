//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXObject.h" 
#include "LXRenderPass.h"
#include "LXMaterialNode.h"

struct LXVSSignature;
class LXMaterialD3D11;
class LXConstantBuffer;
class LXFilepath;

class LXShaderFactory : public LXObject
{
public:

	LXShaderFactory();
	virtual ~LXShaderFactory();

	//
	// Generate a HLSL shader file according the signature and role.
	//

	static void GenerateVertexShader(const LXFilepath& Filename, const LXMaterialD3D11* materialD3D11, ERenderPass renderPass, int layoutMask);

	//
	// Generate a HLSL shader file according...
	//

	static void GeneratePixelShader(const LXFilepath& Filename, const LXMaterialD3D11* materialD3D11, ERenderPass renderPass);
	
	//
	// Update an existing shader file contain:
	// with a generated HLSL declaration according the material textures and ConstantBuffers.
	// only between (and if exists) //BEGIN_GENERATED //END_GENERATED 
	//

	static void UpdateShader(const LXFilepath& Filepath, const LXMaterialD3D11* MaterialD3D11);
			
	//
	// Generated a shader filename.
	//

	static LXString BuildShaderFilename(const LXString& BaseName, ERenderPass RenderPass, int LayoutMask, EShader Shader);

private:

	//
	// Helpful functions to create the generated shader filename.
	//
	static LXString GetRenderPassName(ERenderPass RenderPass);
	static LXString GetShaderName(EShader Shader);
	static LXString GetLayoutName(int LayoutMask);

	//
	// Generate a ConstantBuffer declaration
	//

	static LXStringA ConstantBufferToHLSL(const LXConstantBuffer& ConstantBuffer);

	//
	// Generate the Textures and SamplerStates declaration
	//

	static LXStringA ListTexturesToHLSL(const list<LXTextureD3D11*>& listTextures, EShader shader);
	
	//
	// Generate all Shaders for the given material
	//
	static void GenerateShader(const LXMaterial* Material, ERenderPass RenderPass);


};

