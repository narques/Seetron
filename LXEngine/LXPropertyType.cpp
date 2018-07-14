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
	
	CHK(0);
	return EPropertyType::Undefined;
}

EConnectorType GetConnectorTypeFromName(const wchar_t* typeName)
{
	CMP_CTYPE(Float);
	CMP_CTYPE(Float2);
	CMP_CTYPE(Float3);
	CMP_CTYPE(Float4);
	CMP_CTYPE(Texture);
	CMP_CTYPE(Sampler);
	
	CHK(0);
	return EConnectorType::Undefined;
}
