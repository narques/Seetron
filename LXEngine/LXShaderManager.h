//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXCore/LXObject.h"
#include "LXRenderPassType.h"
#include "LXMaterialNode.h"

class LXFileWatcher;
class LXMaterialD3D11;
class LXPrimitiveD3D11;
class LXShader;
class LXShaderD3D11;
class LXShaderProgramD3D11;
struct D3D11_INPUT_ELEMENT_DESC;
enum class EShaderType;

struct LXVSSignature
{
	const LXMaterialD3D11* Material = nullptr;
	D3D11_INPUT_ELEMENT_DESC* Layout;	// Layout description array pointer
	UINT LayoutElements;				// Layout array element count
	int LayoutMask;						// Layout Mask TODO : TO CLARIFY
	ERenderPass RenderPass;
};

struct LXHSSignature
{
	const LXMaterialD3D11* Material = nullptr;
};

struct LXDSSignature
{
	const LXMaterialD3D11* Material = nullptr;
};

struct LXGSSignature
{
	const LXMaterialD3D11* Material = nullptr;
};

struct LXPSSignature
{
	const LXMaterialD3D11* Material = nullptr; 
	ERenderPass RenderPass;
};

struct CmpLXVSSignature{
	bool operator()(const LXVSSignature& a, const LXVSSignature& b) const
	{
		if (a.Layout < b.Layout)
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
		return a.Material > b.Material;
	}
};

struct CmpLXDSSignature {
	bool operator()(const LXDSSignature& a, const LXDSSignature& b) const
	{
		return a.Material > b.Material;
	}
};

struct CmpLXGSSignature {
	bool operator()(const LXGSSignature& a, const LXGSSignature& b) const
	{
		return a.Material > b.Material;
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

typedef std::map<LXVSSignature, std::shared_ptr<LXShaderD3D11>, CmpLXVSSignature> MapVertexShaders;
typedef std::map<LXHSSignature, std::shared_ptr<LXShaderD3D11>, CmpLXHSSignature> MapHullShaders;
typedef std::map<LXDSSignature, std::shared_ptr<LXShaderD3D11>, CmpLXDSSignature> MapDomainShaders;
typedef std::map<LXGSSignature, std::shared_ptr<LXShaderD3D11>, CmpLXGSSignature> MapGeometryShaders;
typedef std::map<LXPSSignature, std::shared_ptr<LXShaderD3D11>, CmpLXPSSignature> MapPixelShaders;

class LXShaderManager : public LXObject
{

public:

	LXShaderManager();
	virtual ~LXShaderManager();
	
	void CreateDefaultShaders();

	void RebuildShaders();
	void DeleteUnusedShaders(bool keepErroneous = false);

	bool GetShaderSimple(LXShaderD3D11* OutVS, LXShaderD3D11* OutPS);

	// Retrieve shaders according the giving Material
	bool GetShaders(ERenderPass RenderPass, const LXPrimitiveD3D11* InPrimitive, const LXMaterialD3D11* InMaterial, LXShaderProgramD3D11* OutShaderProgram);

	// Release shaders for the given Material
	void ReleaseShaders(const LXMaterialD3D11* material);
	   
	// Retrieve Shader according the giving Material
	LXShaderD3D11* GetTextureShader(const LXString& ShaderFilename);

	// Add a shader file to the monitoring feature,
	// the modified shaders will ne automatically reloaded and rebuilt.
	void AddMonitoredShaderFile(const std::wstring& filename, LXShaderD3D11* shaderD3D11);

	void Run();

private:

	template<typename T, typename M>
	std::shared_ptr<LXShaderD3D11> FindOrCreate(T& Signature, M& Shaders);

	template<typename M>
	void DeleteUnusedShaders(M& mapShaders, bool keepErroneous);

	template<typename M>
	void ReleaseShaders(const LXMaterialD3D11* material, M& mapShaders);
	
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

private:

	std::unique_ptr<LXFileWatcher> _engineFileWatcher;
	std::map<std::wstring, std::list<LXShaderD3D11*>> _monitoredShaderFiles;
	std::set< LXShaderD3D11*> _shaderToRebuild;
};

