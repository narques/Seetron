//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXVec2.h"
#include "LXVec3.h"
#include "LXVec4.h"
#include "LXMatrix.h"

enum class EHLSLType
{
	HLSL_float,
	HLSL_float2,
	HLSL_float3,
	HLSL_float4,
	HLSL_Matrix
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

	bool  BuilldHLSL();
	const LXStringA& GetHLSLDeclaration() const { return HLSLDeclaration; }

	void* GetData() { return &*Buffer.begin(); }
	bool HasData() const;
	uint GetSize();

	void Update(const LXString& Name, const float& Value);
	void Update(const LXString& Name, const vec4f& Value);
		
private:

	bool IsNameFree(const LXStringA& Name);
	void DoPad();

	template<typename T>
	bool AddVariable(EHLSLType inType, const LXString& Name, const T& Value);

private:

	struct LXVariableDeclaration
	{
		EHLSLType Type;
		LXStringA Name;
		uint Offset;
	};

public:

	bool ValueHasChanged = false;

private:

	list<LXVariableDeclaration> VariableDeclarations;
	std::vector<unsigned char> Buffer;
	LXStringA HLSLDeclaration;
};

