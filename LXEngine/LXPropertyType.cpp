//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "StdAfx.h"
#include "LXPropertyType.h"

#define CMP_PTYPE(type) if (wcscmp(typeName, L#type) == 0) return EPropertyType::type;
#define CMP_CTYPE(type) if (wcscmp(typeName, L#type) == 0) return EConnectorType::type;

EPropertyType GetPropertyTypeFromName(const wchar_t* typeName)
{
	CMP_PTYPE(Bool);
	CMP_PTYPE(Double);
	CMP_PTYPE(Float);
	CMP_PTYPE(Float2);
	CMP_PTYPE(Float3);
	CMP_PTYPE(Float4);
	CMP_PTYPE(Int);
	CMP_PTYPE(Uint);
	CMP_PTYPE(Enum);
	CMP_PTYPE(Color);
	CMP_PTYPE(Matrix);
	CMP_PTYPE(String);
	CMP_PTYPE(Filepath);
	CMP_PTYPE(AssetPtr);
	CMP_PTYPE(MaterialNode);
	CMP_PTYPE(ArrayFloat3f);
	CMP_PTYPE(ArraySmartObject);
	CMP_PTYPE(SmartObject);
	
	if (wcscmp(typeName, L"Texture2D") == 0) 
		return EPropertyType::AssetPtr;

	if (wcscmp(typeName, L"ConstantFloat") == 0)
		return EPropertyType::Float;

	if (wcscmp(typeName, L"ConstantFloat2") == 0)
		return EPropertyType::Float2;

	if (wcscmp(typeName, L"ConstantFloat3") == 0)
		return EPropertyType::Float3;

	if (wcscmp(typeName, L"ConstantFloat4") == 0)
		return EPropertyType::Float4;
		
	CHK(0);
	return EPropertyType::Undefined;
}

EConnectorType GetConnectorTypeFromName(const wchar_t* typeName)
{
	CMP_CTYPE(Float);
	CMP_CTYPE(Float2);
	CMP_CTYPE(Float3);
	CMP_CTYPE(Float4);
	CMP_CTYPE(Texture2D);
	CMP_CTYPE(SamplerState);
			
	CHK(0);
	LogE(LXPropertyType, L"GetConnectorTypeFromName: undefined type %s", typeName);
	
	return EConnectorType::Undefined;
}
