//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

enum class EHLSLType
{
	HLSL_float,
	HLSL_float2,
	HLSL_float3,
	HLSL_float4,
	HLSL_Matrix
};

struct LXVariableDeclaration
{
	EHLSLType Type;
	LXStringA Name;
	uint Offset;
	uint Size;
	void* Value;
};

class LXConstantBuffer 
{

public:

	LXConstantBuffer();
	~LXConstantBuffer();
	
	void Release();

	bool AddFloat(const LXString& Name, const float& Value);
	bool AddFloat2(const LXString& Name, const vec2f& Value);
	bool AddFloat3(const LXString& Name, const vec3f& Value);
	bool AddFloat4(const LXString& Name, const vec4f& Value);
	bool AddMatrix(const LXString& Name, const LXMatrix& Value);

	const std::list<LXVariableDeclaration>&  GetVariables() const { return VariableDeclarations; }
	const void* GetData() const { return &*Buffer.begin(); }
	bool HasData() const;

	uint GetSize() const;

	uint GetPadSize() const 
	{
		return _PadSize;
	}

	void UpdateAll();
	void Update(const LXString& Name, const float& Value);
	void Update(const LXString& Name, const vec4f& Value);
		
private:

	bool IsNameFree(const LXStringA& Name, EHLSLType type);
	void AddPad();
	void RemovePad();
	
	template<typename T>
	bool AddVariable(EHLSLType inType, const LXString& Name, const T& Value);

public:

	mutable bool ValueHasChanged = false;

private:

	std::list<LXVariableDeclaration> VariableDeclarations;
	std::vector<unsigned char> Buffer;
	uint _PadSize = 0;
};

