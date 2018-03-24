//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "LXRenderTargetViewD3D11.h"
#include "LXTextureD3D11.h"
#include "LXDirectX11.h"
#include "LXStatistic.h"
#include "LXMemory.h" // --- Must be the last included ---

LXRenderTargetViewD3D11::LXRenderTargetViewD3D11(LXTextureD3D11* Texture)
{
	LX_COUNTSCOPEINC(LXRenderTargetViewD3D11)

	DXGI_FORMAT FormatRTV;
	DXGI_FORMAT FormatSRV;
	LXTextureD3D11::GetFormats((DXGI_FORMAT)Texture->_Format, FormatRTV, FormatSRV);

	// Create the render target view
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	renderTargetViewDesc.Format = FormatRTV;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;

	LXDirectX11* DirectX11 = LXDirectX11::GetCurrentDirectX11();
	HRESULT hr = DirectX11->_D3D11Device->CreateRenderTargetView(Texture->D3D11Texture2D, &renderTargetViewDesc, &D3D11RenderTargetView);
	if (FAILED(hr))
		CHK(0);
}

LXRenderTargetViewD3D11::~LXRenderTargetViewD3D11()
{
	LX_COUNTSCOPEDEC(LXRenderTargetViewD3D11)
	CHK(IsRenderThread())
	LX_SAFE_RELEASE(D3D11RenderTargetView);
}

LXDepthStencilViewD3D11::LXDepthStencilViewD3D11(LXTextureD3D11* Texture)
{
	LX_COUNTSCOPEINC(LXDepthStencilViewD3D11)

	DXGI_FORMAT FormatRTV;
	DXGI_FORMAT FormatSRV;
	LXTextureD3D11::GetFormats((DXGI_FORMAT)Texture->_Format, FormatRTV, FormatSRV);
		
	// Create the render target view
	D3D11_DEPTH_STENCIL_VIEW_DESC DepthStencilViewDesc;
	DepthStencilViewDesc.Flags = 0;
	DepthStencilViewDesc.Format = FormatRTV;
	DepthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	DepthStencilViewDesc.Texture2D.MipSlice = 0;

	LXDirectX11* DirectX11 = LXDirectX11::GetCurrentDirectX11();
	HRESULT hr = DirectX11->_D3D11Device->CreateDepthStencilView(Texture->D3D11Texture2D, &DepthStencilViewDesc, &D3D11DepthStencilView);
	if (FAILED(hr))
		CHK(0);
}

LXDepthStencilViewD3D11::~LXDepthStencilViewD3D11()
{
	LX_COUNTSCOPEDEC(LXDepthStencilViewD3D11)
	LX_SAFE_RELEASE(D3D11DepthStencilView);
}
