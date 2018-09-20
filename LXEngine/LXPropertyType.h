//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

enum class EPropertyType
{
	Undefined,
	Bool,
	Double,
	Float,
	Float2,
	Float3,
	Float4,
	Int,
	Uint,
	Enum,
	Color,
	Matrix,
	String,
	Filepath,
	AssetPtr,
	MaterialNode,
	ArrayFloat3f,
	ArraySmartObject,
	ListSmartObject,
	SmartObject,
	SharedObject,
	ReferenceObject
};

enum class EConnectorType
{
	Undefined,
	Float,
	Float2,
	Float3,
	Float4,
	Texture2D,
	SamplerState,
};

EPropertyType GetPropertyTypeFromName(const wchar_t* typeName);
EConnectorType GetConnectorTypeFromName(const wchar_t* typeName);

