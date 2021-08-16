//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "LXConstantBuffer.h"
#include "LXLogger.h"

LXConstantBuffer::LXConstantBuffer()
{
}

LXConstantBuffer::~LXConstantBuffer()
{
}

void LXConstantBuffer::Release()
{
	Buffer.clear();
	VariableDeclarations.clear();
}

bool LXConstantBuffer::IsNameFree(const LXStringA& Name, EHLSLType type)
{

	for (const LXVariableDeclaration& VD: VariableDeclarations)
	{
		if (VD.Name == Name)
		{
			if (VD.Type != type)
			{
				LogD(ConstantBuffer, "'%s' variable name is already used in ConstantBuffer structure with a different type.", Name.GetBuffer());
				CHK(0);
				return false;
			}

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

void LXConstantBuffer::UpdateAll()
{
	for (const LXVariableDeclaration& VD : VariableDeclarations)
	{
		uint Offset = VD.Offset;
		void* p = &Buffer[Offset];
		if (memcmp(p, VD.Value, VD.Size) != 0)
		{
			memcpy(p, VD.Value, VD.Size);
			ValueHasChanged = true;
		}
		else
		{
			int foo = 0;
		}
	}
}

void LXConstantBuffer::Update(const LXString& Name, const float& Value)
{
	LXStringA nameA = Name.ToStringA();
	for (const LXVariableDeclaration& VD : VariableDeclarations)
	{
		if (VD.Name == nameA)
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
	LXStringA nameA = Name.ToStringA();
	for (const LXVariableDeclaration& VD : VariableDeclarations)
	{
		if (VD.Name == nameA)
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
	LXStringA nameA = Name.ToStringA();

	if (!IsNameFree(nameA, inType))
		return true;

	RemovePad();
 
	uint Offset = (uint)Buffer.size();
	Buffer.resize(Offset + sizeof(T));
	T* p = (T*)&Buffer[Offset];
	*p = Value;

	AddPad();

	LXVariableDeclaration NewVariable;
	NewVariable.Type = inType;
	NewVariable.Name = nameA;
	NewVariable.Offset = Offset;
	NewVariable.Size = sizeof(T);
	NewVariable.Value = (void*)&Value;

	VariableDeclarations.push_back(NewVariable);

	return true;
 }

bool LXConstantBuffer::HasData() const
{
	return Buffer.size() != 0;
}

uint LXConstantBuffer::GetSize() const
{
	return (uint)Buffer.size();
}

void LXConstantBuffer::RemovePad()
{
	uint Size = (uint)Buffer.size();
	if (_PadSize > 0)
	{
		Buffer.resize(Size - _PadSize);
		_PadSize = 0;
	}
}

void LXConstantBuffer::AddPad()
{
	CHK(_PadSize == 0); // Pad already pushed

	uint Size = (uint)Buffer.size();

	uint to48Bytes = Size >= 48 ? 0 : 48 - Size;

	_PadSize = ((16 - ((Size + to48Bytes) % 16)) % 16) + to48Bytes;
	if (_PadSize > 0)
	{
		Buffer.resize(Size + _PadSize);
	}
}
