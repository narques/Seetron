//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXObject.h"
#include "LXDirectX11.h"

typedef pair<std::string, std::string> LXShaderMacro;

enum class EShaderD3D11State
{
	Unknown,
	Ok,
	CompilationError,
	CreationError
};

enum class EShaderType
{
	Undefined,
	VertexShader,
	HullShader,
	DomainShader,
	GeometryShader,
	PixelShader
};

class LXShaderD3D11 : public LXObject
{

public:

	LXShaderD3D11();
	virtual ~LXShaderD3D11();

	void AddMacro(const char* Name, const char* Definition);

	bool CreateVertexShader(wchar_t* Filename, const D3D11_INPUT_ELEMENT_DESC* Layout, UINT NumElements);
	bool CreateHullShader(wchar_t* Filename);
	bool CreateDomainShader(wchar_t* Filename);
	bool CreateGeometryShader(wchar_t* Filename);
	bool CreatePixelShader(wchar_t* Filename, const char* EntryPoint = "PS");

	EShaderD3D11State GetState() const { return State; }

	bool IsValid() const { return State == EShaderD3D11State::Ok; }

private:

	HRESULT CompileShaderFromFile(wchar_t* Filename, const char* szEntryPoint, const char* szShaderModel, ID3DBlob** ppBlobOut);

public:
		
	ID3D11VertexShader* D3D11VertexShader = nullptr;
	ID3D11HullShader* D3D11HullShader = nullptr;
	ID3D11DomainShader* D3D11DomainShader = nullptr;
	ID3D11GeometryShader* D3D11GeometryShader = nullptr;
	ID3D11PixelShader* D3D11PixelShader = nullptr;

	ID3D11InputLayout* D3D11VertexLayout = nullptr;

private:

	vector<LXShaderMacro> ShaderMacros;
	EShaderD3D11State State = EShaderD3D11State::Unknown;
	EShaderType Type = EShaderType::Undefined;
};

