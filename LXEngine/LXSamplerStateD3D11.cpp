//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "pch.h"
#include "LXSamplerStateD3D11.h"
#include "LXStatistic.h"
#include "LXTextureSampler.h"
#include "LXThreadManager.h"

LXSamplerStateD3D11::LXSamplerStateD3D11()
{
	LX_COUNTSCOPEINC(LXSamplerStateD3D11);
	CHK(IsRenderThread());
	Create(D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_WRAP, D3D11_TEXTURE_ADDRESS_WRAP, D3D11_TEXTURE_ADDRESS_WRAP);
}

LXSamplerStateD3D11::LXSamplerStateD3D11(D3D11_FILTER filter, D3D11_TEXTURE_ADDRESS_MODE addressU, D3D11_TEXTURE_ADDRESS_MODE addressV, D3D11_TEXTURE_ADDRESS_MODE addressW)
{
	LX_COUNTSCOPEINC(LXSamplerStateD3D11)
	CHK(IsRenderThread());
	Create(filter, addressU, addressV, addressW);
}

LXSamplerStateD3D11::~LXSamplerStateD3D11()
{
	LX_COUNTSCOPEDEC(LXSamplerStateD3D11)
	CHK(IsRenderThread());
	LX_SAFE_RELEASE(D3D11SamplerState);
}

void LXSamplerStateD3D11::Create(
	D3D11_FILTER filter,
	D3D11_TEXTURE_ADDRESS_MODE addressU,
	D3D11_TEXTURE_ADDRESS_MODE addressV,
	D3D11_TEXTURE_ADDRESS_MODE addressW)
{
	ID3D11Device* D3D11Device = LXDirectX11::GetCurrentDevice();
	ID3D11DeviceContext* D3D11DeviceContext = LXDirectX11::GetCurrentDeviceContext();

	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(D3D11_SAMPLER_DESC));
	sampDesc.Filter = filter;
	sampDesc.AddressU = addressU,
	sampDesc.AddressV = addressV;
	sampDesc.AddressW = addressW;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	HRESULT hr = D3D11Device->CreateSamplerState(&sampDesc, &D3D11SamplerState);
	if (FAILED(hr))
		CHK(0);
}

LXSamplerStateD3D11* LXSamplerStateD3D11::CreateFromSampler(LXTextureSampler* textureSampler)
{
	// TextureSampler enum values match DX11 enum.
	D3D11_FILTER filter = (D3D11_FILTER)textureSampler->GetFilter();
	D3D11_TEXTURE_ADDRESS_MODE addressU = (D3D11_TEXTURE_ADDRESS_MODE)textureSampler->GetAddressU();
	D3D11_TEXTURE_ADDRESS_MODE addressV = (D3D11_TEXTURE_ADDRESS_MODE)textureSampler->GetAddressV();
	D3D11_TEXTURE_ADDRESS_MODE addressW = (D3D11_TEXTURE_ADDRESS_MODE)textureSampler->GetAddressW();
	return new LXSamplerStateD3D11(filter, addressU, addressV, addressW);
}
