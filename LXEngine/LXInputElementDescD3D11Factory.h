//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once
#include "LXPrimitive.h"
#include <d3d11.h>

typedef std::vector<D3D11_INPUT_ELEMENT_DESC> LXArrayInputElementDesc;

enum class EPrimitiveLayout
{
	P = LX_PRIMITIVE_POSITIONS,
	PT = (LX_PRIMITIVE_POSITIONS | LX_PRIMITIVE_TEXCOORDS),
	PN = (LX_PRIMITIVE_POSITIONS | LX_PRIMITIVE_NORMALS),
	PNT = (LX_PRIMITIVE_POSITIONS | LX_PRIMITIVE_NORMALS | LX_PRIMITIVE_TEXCOORDS),
	PNABT = (LX_PRIMITIVE_POSITIONS | LX_PRIMITIVE_NORMALS | LX_PRIMITIVE_TANGENTS | LX_PRIMITIVE_BINORMALS | LX_PRIMITIVE_TEXCOORDS),
	PNABTI = (LX_PRIMITIVE_POSITIONS | LX_PRIMITIVE_NORMALS | LX_PRIMITIVE_TANGENTS | LX_PRIMITIVE_BINORMALS | LX_PRIMITIVE_TEXCOORDS | LX_PRIMITIVE_INSTANCEPOSITIONS)
};

class LXCORE_API LXInputElementDescD3D11Factory
{

public:

	LXInputElementDescD3D11Factory();
	~LXInputElementDescD3D11Factory();
	
	const LXArrayInputElementDesc& GetInputElement(int Mask);
	const LXArrayInputElementDesc& GetInputElement_PT();

private:

	std::map<int, LXArrayInputElementDesc> MapElements;

};

LXCORE_API LXInputElementDescD3D11Factory& GetInputElementDescD3D11Factory();

