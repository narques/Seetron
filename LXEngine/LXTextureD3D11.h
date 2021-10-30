//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXCore/LXObject.h"
#include "LXDirectX11.h"
#include "LXFormats.h"

class LXRenderCommandList;
class LXBitmap;
class LXTexture;

class LXENGINE_API LXTextureD3D11 : public LXObject
{

public:

	// Buffers
	LXTextureD3D11();
	LXTextureD3D11(uint Width, uint Height, DXGI_FORMAT Format, bool bSupportAutoMipmap = false);
	void Create(DXGI_FORMAT Format, uint Width, uint Height, bool bSupportAutoMipmap);
	
	// Textures
	LXTextureD3D11(uint Width, uint Height, DXGI_FORMAT Format, void* Buffer, uint PixelSize, uint MipLevels, D3D11_FILTER Filter = D3D11_FILTER_ANISOTROPIC);
	
	virtual ~LXTextureD3D11();

	ID3D11RenderTargetView* GetRenderTargetView() const;

	// Helpers
	static LXTextureD3D11* CreateFromTexture(LXTexture* InTexture);
		
	void CreateForCapture2(ID3D11Texture2D*SrcResource);
	void ReadToBitmap(LXRenderCommandList* RCL, LXBitmap* Bitmap);
	
	static DXGI_FORMAT GetDXGIFormat(ETextureFormat TextureFormat);
	static DXGI_FORMAT GetFormatSRV(DXGI_FORMAT TextureFormatDXGI);
	static void GetFormats(DXGI_FORMAT FormatTexture, DXGI_FORMAT& OutFormatRTV, DXGI_FORMAT& OutFormatSRV);
	static void SwapDeviceObjects(LXTextureD3D11* a, LXTextureD3D11* b);
	
public:

	ID3D11Texture2D* D3D11Texture2D = nullptr;
	ID3D11ShaderResourceView* D3D11ShaderResouceView = nullptr;

	DXGI_FORMAT _Format;

private:

	// Created on demand if the texture is used as a RenderTarget.
	mutable ID3D11RenderTargetView* D3D11RenderTargetView = nullptr;
};


