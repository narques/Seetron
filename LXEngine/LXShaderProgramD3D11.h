//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXObject.h"

class LXShaderD3D11;
class LXRenderCommandList;
struct D3D11_INPUT_ELEMENT_DESC;

class LXShaderProgramD3D11 : public LXObject
{

public:

	LXShaderProgramD3D11();
	virtual ~LXShaderProgramD3D11();
	
	void Release()
	{
		HullShader.reset();
		DomainShader.reset();
		GeometryShader.reset();
		VertexShader.reset();
		PixelShader.reset();
	}
	
public:

	shared_ptr<LXShaderD3D11> HullShader;
	shared_ptr<LXShaderD3D11> DomainShader;
	shared_ptr<LXShaderD3D11> GeometryShader;
	shared_ptr<LXShaderD3D11> VertexShader;
	shared_ptr<LXShaderD3D11> PixelShader;
};

//------------------------------------------------------------------------------------------------------
// Encapsulates a VertexShader and a PixelShader in a single object
//------------------------------------------------------------------------------------------------------

class LXShaderProgramBasic
{

public:

	LXShaderProgramBasic();
	~LXShaderProgramBasic();

	bool CreateShaders(const wchar_t* Filename, const D3D11_INPUT_ELEMENT_DESC* Layout, UINT NumElements);
	bool IsValid() const;
	void Render(LXRenderCommandList* r) const;

	LXShaderD3D11* VertexShader;
	LXShaderD3D11* PixelShader;
	
};