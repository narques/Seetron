//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXCore/LXObject.h"
#include "LXDirectX11.h"

typedef std::pair<std::string, std::string> LXShaderMacro;

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

	void AddMacro(const char* name, const char* definition);

	bool Create();

	bool CreateVertexShader(const wchar_t* filename, const D3D11_INPUT_ELEMENT_DESC* Layout, UINT NumElements);
	bool CreateHullShader(const wchar_t* filename);
	bool CreateDomainShader(const wchar_t* filename);
	bool CreateGeometryShader(const wchar_t* filename);
	bool CreatePixelShader(const wchar_t* filename, const char* EntryPoint = "PS");

	EShaderD3D11State GetState() const { return State; }

	bool IsValid() const { return State == EShaderD3D11State::Ok; }

private:

	HRESULT CompileShaderFromFile(const wchar_t* filename, const char* szEntryPoint, const char* szShaderModel, ID3DBlob** ppBlobOut);

public:
		
	ID3D11VertexShader* D3D11VertexShader = nullptr;
	ID3D11HullShader* D3D11HullShader = nullptr;
	ID3D11DomainShader* D3D11DomainShader = nullptr;
	ID3D11GeometryShader* D3D11GeometryShader = nullptr;
	ID3D11PixelShader* D3D11PixelShader = nullptr;

	ID3D11InputLayout* D3D11VertexLayout = nullptr;

private:

	std::vector<LXShaderMacro> ShaderMacros;
	EShaderD3D11State State = EShaderD3D11State::Unknown;
	EShaderType Type = EShaderType::Undefined;
	std::wstring _filename;
		
	// VertexShader
	const D3D11_INPUT_ELEMENT_DESC* _layout = nullptr;
	UINT _numElements = 0;

	// PixelShader
	const char* _entryPoint = nullptr;
};

