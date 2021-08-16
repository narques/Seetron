//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "LXInputElementDescD3D11Factory.h"
#include "LXPrimitive.h"

LXInputElementDescD3D11Factory::LXInputElementDescD3D11Factory()
{
}

LXInputElementDescD3D11Factory::~LXInputElementDescD3D11Factory()
{
}

D3D11_INPUT_ELEMENT_DESC SetPosition()
{
	D3D11_INPUT_ELEMENT_DESC InputElementDesc;

	InputElementDesc.SemanticName = "POSITION";
	InputElementDesc.SemanticIndex = 0;
	InputElementDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
	InputElementDesc.InputSlot = 0;
	InputElementDesc.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	InputElementDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	InputElementDesc.InstanceDataStepRate = 0;

	return InputElementDesc;
}

D3D11_INPUT_ELEMENT_DESC SetNormal()
{
	D3D11_INPUT_ELEMENT_DESC InputElementDesc;

	InputElementDesc.SemanticName = "NORMAL";
	InputElementDesc.SemanticIndex = 0;
	InputElementDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
	InputElementDesc.InputSlot = 0;
	InputElementDesc.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	InputElementDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	InputElementDesc.InstanceDataStepRate = 0;

	return InputElementDesc;
}

D3D11_INPUT_ELEMENT_DESC SetTangent()
{
	D3D11_INPUT_ELEMENT_DESC InputElementDesc;

	InputElementDesc.SemanticName = "TANGENT";
	InputElementDesc.SemanticIndex = 0;
	InputElementDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
	InputElementDesc.InputSlot = 0;
	InputElementDesc.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	InputElementDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	InputElementDesc.InstanceDataStepRate = 0;

	return InputElementDesc;
}

D3D11_INPUT_ELEMENT_DESC SetBinormal()
{
	D3D11_INPUT_ELEMENT_DESC InputElementDesc;

	InputElementDesc.SemanticName = "BINORMAL";
	InputElementDesc.SemanticIndex = 0;
	InputElementDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
	InputElementDesc.InputSlot = 0;
	InputElementDesc.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	InputElementDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	InputElementDesc.InstanceDataStepRate = 0;

	return InputElementDesc;
}

D3D11_INPUT_ELEMENT_DESC SetTexcoord()
{
	D3D11_INPUT_ELEMENT_DESC InputElementDesc;

	InputElementDesc.SemanticName = "TEXCOORD";
	InputElementDesc.SemanticIndex = 0;
	InputElementDesc.Format = DXGI_FORMAT_R32G32_FLOAT;
	InputElementDesc.InputSlot = 0;
	InputElementDesc.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	InputElementDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	InputElementDesc.InstanceDataStepRate = 0;

	return InputElementDesc;
}

D3D11_INPUT_ELEMENT_DESC SetInstancePosition()
{
	D3D11_INPUT_ELEMENT_DESC InputElementDesc;
	
	InputElementDesc.SemanticName = "INSTANCEPOSITION";
	InputElementDesc.SemanticIndex = 0;
	InputElementDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
	InputElementDesc.InputSlot = 1;
	InputElementDesc.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	InputElementDesc.InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
	InputElementDesc.InstanceDataStepRate = 1;

	return InputElementDesc;
}

const LXArrayInputElementDesc& LXInputElementDescD3D11Factory::GetInputElement(int Mask)
{
	auto It = MapElements.find(Mask);
	if (It != MapElements.end())
		return It->second;
	
	LXArrayInputElementDesc& ArrayInputElemenDesc = MapElements[Mask];
	CHK(ArrayInputElemenDesc.size() == 0);


	if (Mask & LX_PRIMITIVE_POSITIONS)
	{
		ArrayInputElemenDesc.push_back(SetPosition());
	}

	if (Mask & LX_PRIMITIVE_NORMALS)
	{
		ArrayInputElemenDesc.push_back(SetNormal());
	}

	if (Mask & LX_PRIMITIVE_TANGENTS)
	{
		ArrayInputElemenDesc.push_back(SetTangent());
	}

	if (Mask & LX_PRIMITIVE_BINORMALS)
	{
		ArrayInputElemenDesc.push_back(SetBinormal());
	}

	if (Mask & LX_PRIMITIVE_TEXCOORDS)
	{
		ArrayInputElemenDesc.push_back(SetTexcoord());
	}

	if (Mask & LX_PRIMITIVE_INSTANCEPOSITIONS)
	{
		ArrayInputElemenDesc.push_back(SetInstancePosition());
	}

	return ArrayInputElemenDesc;
}

const LXArrayInputElementDesc& LXInputElementDescD3D11Factory::GetInputElement_PT()
{
	return GetInputElement(LX_PRIMITIVE_POSITIONS | LX_PRIMITIVE_TEXCOORDS);
}

LXInputElementDescD3D11Factory& GetInputElementDescD3D11Factory()
{
	static LXInputElementDescD3D11Factory InputElementDescD3D11Factory;
	return InputElementDescD3D11Factory;
}

