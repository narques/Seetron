//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "LXConstantBuffer.h"
#include "LXMemory.h" // --- Must be the last included ---

LXConstantBuffer::LXConstantBuffer()
{
}

LXConstantBuffer::~LXConstantBuffer()
{
}

void LXConstantBuffer::Release()
{
	Buffer.clear();
	HLSLDeclaration = "";
	VariableDeclarations.clear();
}

bool LXConstantBuffer::IsNameFree(const LXStringA& Name)
{

	for (const LXVariableDeclaration& VD: VariableDeclarations)
	{
		if (VD.Name == Name)
		{
			LogD(ConstantBuffer, L"'%s' variable name is already used in ConstantBuffer structure.", Name.GetBuffer());
			return false;
		}
	}
	return true;
}

bool LXConstantBuffer::AddFloat(const LXString& Name, const float& Value)
{
	return AddVariable(EHLSLType::HLSL_float, Name, Value);
}

bool LXConstantBuffer::AddFloat2(const LXString& Name, const vec2f& Value)
{
	return AddVariable(EHLSLType::HLSL_float2, Name, Value);
}

bool LXConstantBuffer::AddFloat3(const LXString& Name, const vec3f& Value)
{
	return AddVariable(EHLSLType::HLSL_float3, Name, Value);
}

bool LXConstantBuffer::AddFloat4(const LXString& Name, const vec4f& Value)
{
	return AddVariable(EHLSLType::HLSL_float4, Name, Value);
}

bool LXConstantBuffer::AddMatrix(const LXString& Name, const LXMatrix& Value)
{
	return AddVariable(EHLSLType::HLSL_Matrix, Name, Value);
}

void LXConstantBuffer::Update(const LXString& Name, const float& Value)
{
	LXStringA NameA = &*Name.GetBufferA().begin();
	for (const LXVariableDeclaration& VD : VariableDeclarations)
	{
		if (VD.Name == NameA)
		{
			uint Offset = VD.Offset;
			float* p = (float*)&Buffer[Offset];
			if (*p != Value)
			{
				*p = Value;
				ValueHasChanged = true;
			}
			return;
		}
	}
	
	// Variable not found
	CHK(0);
}

void LXConstantBuffer::Update(const LXString& Name, const vec4f& Value)
{
	LXStringA NameA = &*Name.GetBufferA().begin();
	for (const LXVariableDeclaration& VD : VariableDeclarations)
	{
		if (VD.Name == NameA)
		{
			uint Offset = VD.Offset;
			vec4f* p = (vec4f*)&Buffer[Offset];
			if (*p != Value)
			{
				*p = Value;
				ValueHasChanged = true;
			}
			return;
		}
	}

	// Variable not found
	CHK(0); 
}

template<class T>
bool LXConstantBuffer::AddVariable(EHLSLType inType, const LXString& Name, const T& Value)
{
	LXStringA NameA = &*Name.GetBufferA().begin();

	if (!IsNameFree(NameA))
		return false;
 
	uint Offset = (uint)Buffer.size();
	Buffer.resize(Offset + sizeof(T));
 
	T* p = (T*)&Buffer[Offset];
	*p = Value;

	LXVariableDeclaration NewVariable;
	NewVariable.Type = inType;
	NewVariable.Name = NameA;
	NewVariable.Offset = Offset;

	VariableDeclarations.push_back(NewVariable);

	return true;
 }

bool LXConstantBuffer::HasData() const
{
	return Buffer.size() != 0;
}

uint LXConstantBuffer::GetSize()
{
	return (uint)Buffer.size();
}

void LXConstantBuffer::DoPad()
{
	uint Size = (uint)Buffer.size();
	uint PadSize = (16 - (Size % 16)) % 16;
	if (PadSize > 0)
	{
		Buffer.resize(Size + PadSize);
	}
}

bool LXConstantBuffer::BuilldHLSL()
{
	DoPad();

	HLSLDeclaration += "cbuffer ConstantBuffer_001\n";
	HLSLDeclaration += "{\n";
	
	for (const auto& VD : VariableDeclarations)
	{
		LXStringA TypeName;
		switch (VD.Type)
		{
		case EHLSLType::HLSL_float: TypeName = "float"; break;
		case EHLSLType::HLSL_float2: TypeName = "float2"; break;
		case EHLSLType::HLSL_float3: TypeName = "float3"; break;
		case EHLSLType::HLSL_float4: TypeName = "float4"; break;
		case EHLSLType::HLSL_Matrix: TypeName = "matrix"; break;
		default: CHK(0); return false;
		}

		HLSLDeclaration += "  " + TypeName + " " + VD.Name + ";\n";
	}
			
	HLSLDeclaration += "};\n";
	return true;
}
