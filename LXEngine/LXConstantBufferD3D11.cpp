//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "LXConstantBufferD3D11.h"
#include "LXDirectX11.h"
#include "LXStatistic.h"
#include "LXMemory.h" // --- Must be the last included ---

LXConstantBufferD3D11::LXConstantBufferD3D11()
{
	LX_COUNTSCOPEINC(LXConstantBufferD3D11)
}

LXConstantBufferD3D11::LXConstantBufferD3D11(const void* buffer, int bufferSize)
{
	LX_COUNTSCOPEINC(LXConstantBufferD3D11)
	VRF(CreateConstantBuffer(buffer, bufferSize));
}

LXConstantBufferD3D11::~LXConstantBufferD3D11()
{
	LX_COUNTSCOPEDEC(LXConstantBufferD3D11)
	CHK(IsRenderThread())
	LX_SAFE_RELEASE(D3D11Buffer);
}

bool LXConstantBufferD3D11::CreateConstantBuffer(const void* buffer, int bufferSize)
{
	CHK(((16 - (bufferSize % 16)) % 16) == 0);

	auto *D3D11Device = LXDirectX11::GetCurrentDevice();
	auto *D3D11DeviceContext = LXDirectX11::GetCurrentDeviceContext();

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));

	if (NoOverwriteConstantBuffer)
	{
		bd.Usage = D3D11_USAGE_DYNAMIC;
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	}
	else
	{
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bd.CPUAccessFlags = 0;
	}

	//LXConstantBufferData EmptyConstantBufferData;
	//T EmptyConstantBufferData;
	D3D11_SUBRESOURCE_DATA subdata;
	ZeroMemory(&subdata, sizeof(subdata));
	//subdata.pSysMem = &EmptyConstantBufferData;
	subdata.pSysMem = buffer;

	//bd.ByteWidth = sizeof(LXConstantBufferData);
	//bd.ByteWidth = sizeof(T);
	bd.ByteWidth = bufferSize;

	HRESULT hr = D3D11Device->CreateBuffer(&bd, &subdata, &D3D11Buffer);
	if (FAILED(hr))
	{
		LXDirectX11::LogError(hr);
		CHK(0);
		return false;
	}
	else
	{
		// Init for D3D11_MAP_WRITE_NO_OVERWRITE 
		// 	D3D11_MAPPED_SUBRESOURCE MappedResource;
		// 	if (S_OK == D3D11DeviceContext->Map(ConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource))
		// 	{
		// 		LXConstantBufferData EmptyConstantBufferData;
		// 		memcpy(MappedResource.pData, &EmptyConstantBufferData, sizeof(LXConstantBufferData));
		// 		D3D11DeviceContext->Unmap(ConstantBuffer, 0);
		// 	}

		return true;
	}
}
