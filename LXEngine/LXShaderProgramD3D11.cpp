//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "LXShaderProgramD3D11.h"
#include "LXRenderCommandList.h"
#include "LXShaderD3D11.h"

LXShaderProgramD3D11::LXShaderProgramD3D11()
{
}

LXShaderProgramD3D11::~LXShaderProgramD3D11()
{
}

//------------------------------------------------------------------------------------------------------

LXShaderProgramBasic::LXShaderProgramBasic()
{
	VertexShader = new LXShaderD3D11();
	PixelShader = new LXShaderD3D11();
}

LXShaderProgramBasic::~LXShaderProgramBasic()
{
	delete VertexShader;
	delete PixelShader;
}

bool LXShaderProgramBasic::CreateShaders(const wchar_t* Filename, const D3D11_INPUT_ELEMENT_DESC* Layout, UINT NumElements)
{
	bool bRet = VertexShader->CreateVertexShader(Filename, Layout, NumElements);
	bRet &= PixelShader->CreatePixelShader(Filename);
	return bRet;
}

bool LXShaderProgramBasic::IsValid() const
{
	return PixelShader->IsValid() && VertexShader->IsValid();
}

void LXShaderProgramBasic::Render(LXRenderCommandList* r) const
{
	r->IASetInputLayout(VertexShader);
	r->VSSetShader(VertexShader);
	r->PSSetShader(PixelShader);
}