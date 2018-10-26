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

class LXShader;
class LXPrimitiveD3D11;
class LXMaterialD3D11;
class LXShaderD3D11;
class LXShaderProgramD3D11;
struct D3D11_INPUT_ELEMENT_DESC;

struct LXVSSignature
{
	const LXMaterialD3D11* Material = nullptr;
	uint displacement;					// DEPRECATED
	D3D11_INPUT_ELEMENT_DESC* Layout;	// Layout description array pointer
	UINT LayoutElements;				// Layout array element count
	int LayoutMask;						// Layout Mask TODO : TO CLARIFY
	ERenderPass RenderPass;
};

struct LXHSSignature
{
	uint foo : 1;
};

struct LXDSSignature
{
	uint foo : 1;
};

struct LXGSSignature
{
	uint foo : 1;
};

struct LXPSSignature
{
	const LXMaterialD3D11* Material = nullptr; 
	ERenderPass RenderPass;
};

struct CmpLXVSSignature{
	bool operator()(const LXVSSignature& a, const LXVSSignature& b) const
	{
		if (a.displacement < b.displacement)
			return true;
		else if (a.displacement > b.displacement)
			return false;
		else if (a.Layout < b.Layout)
			return true;
		else if (a.Layout > b.Layout)
			return false;
		else if (a.LayoutElements < b.LayoutElements)
			return true;
		else if (a.LayoutElements > b.LayoutElements)
			return false;
		else if (a.LayoutMask < b.LayoutMask)
			return true;
		else if (a.LayoutMask > b.LayoutMask)
			return false;
		else if (a.Material < b.Material)
			return true;
		else if (a.Material > b.Material)
			return false;
		return ((int)a.RenderPass < (int)b.RenderPass);
	}
};

struct CmpLXHSSignature {
	bool operator()(const LXHSSignature& a, const LXHSSignature& b) const
	{
		return a.foo < b.foo;
	}
};

struct CmpLXDSSignature {
	bool operator()(const LXDSSignature& a, const LXDSSignature& b) const
	{
		return a.foo < b.foo;
	}
};

struct CmpLXGSSignature {
	bool operator()(const LXGSSignature& a, const LXGSSignature& b) const
	{
		return a.foo < b.foo;
	}
};

struct CmpLXPSSignature {
	bool operator()(const LXPSSignature& a, const LXPSSignature& b) const
	{
		if ((int)a.RenderPass < (int)b.RenderPass)
			return true;
		else if ((int)a.RenderPass > (int)b.RenderPass)
			return false;
		return a.Material > b.Material;
	}
};

typedef map<LXVSSignature, shared_ptr<LXShaderD3D11>, CmpLXVSSignature> MapVertexShaders;
typedef map<LXHSSignature, shared_ptr<LXShaderD3D11>, CmpLXHSSignature> MapHullShaders;
typedef map<LXDSSignature, shared_ptr<LXShaderD3D11>, CmpLXDSSignature> MapDomainShaders;
typedef map<LXGSSignature, shared_ptr<LXShaderD3D11>, CmpLXGSSignature> MapGeometryShaders;
typedef map<LXPSSignature, shared_ptr<LXShaderD3D11>, CmpLXPSSignature> MapPixelShaders;

class LXShaderManager : public LXObject
{

public:

	LXShaderManager();
	virtual ~LXShaderManager();
	void RebuildShaders();
	void DeleteUnusedShaders();

	bool GetShaderSimple(LXShaderD3D11* OutVS, LXShaderD3D11* OutPS);

	// Retrieve shaders according the giving Material
	bool GetShaders(ERenderPass RenderPass, const LXPrimitiveD3D11* InPrimitive, const LXMaterialD3D11* InMaterial, LXShaderProgramD3D11* OutShaderProgram);

	// Retrieve Shader according the giving Material
	LXShaderD3D11* GetTextureShader(const LXString& ShaderFilename);

	void Run();

private:

	template<typename T, typename M>
	shared_ptr<LXShaderD3D11> FindOrCreate(T& Signature, M& Shaders);

	bool CreateShader(const LXVSSignature& VSSignature, LXShaderD3D11* Shader);
	bool CreateShader(const LXHSSignature& HSSignature, LXShaderD3D11* Shader);
	bool CreateShader(const LXDSSignature& DSSignature, LXShaderD3D11* Shader);
	bool CreateShader(const LXGSSignature& GSSignature, LXShaderD3D11* Shader);
	bool CreateShader(const LXPSSignature& PSSignature, LXShaderD3D11* Shader);
		
public:

	LXShaderD3D11* VSDrawToBackBuffer;
	LXShaderD3D11* PSDrawToBackBuffer;

	MapVertexShaders VertexShaders;
	MapHullShaders HullShaders;
	MapDomainShaders DomainShaders;
	MapGeometryShaders GeometryShaders;
	MapPixelShaders PixelShaders;
};

