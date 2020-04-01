//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "LXTextureD3D11.h"
#include "LXDirectX11.h"
#include "LXRenderCommandList.h"
#include "LXTexture.h"
#include "LXBitmap.h"
#include "LXStatistic.h"
#include "LXMemory.h" // --- Must be the last included ---

LXTextureD3D11::LXTextureD3D11()
{
	LX_COUNTSCOPEINC(LXTextureD3D11)
	CHK(IsRenderThread())
}

LXTextureD3D11::LXTextureD3D11(uint Width, uint Height, DXGI_FORMAT Format, bool bSupportAutoMipmap)
{
	LX_COUNTSCOPEINC(LXTextureD3D11)
	CHK(IsRenderThread())
	
	Create(Format, Width, Height, bSupportAutoMipmap);
}

LXTextureD3D11::LXTextureD3D11(uint Width, uint Height, DXGI_FORMAT Format, void* Buffer, uint PixelSize, uint MipLevels, D3D11_FILTER Filter)
{
	LX_COUNTSCOPEINC(LXTextureD3D11)
	CHK(IsRenderThread())

	ID3D11Device* D3D11Device = LXDirectX11::GetCurrentDevice();
	ID3D11DeviceContext* D3D11DeviceContext = LXDirectX11::GetCurrentDeviceContext();
		
	D3D11_TEXTURE2D_DESC desc = { 0 };
	desc.Width = Width;
	desc.Height = Height;
	desc.MipLevels = 0; // 0 to Generate 1 to Multisample
	desc.ArraySize = 1;
	desc.Format = (DXGI_FORMAT)Format;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	if (MipLevels > 1)
	{
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET; // D3D11_BIND_RENDER_TARGET Needed for GenerateMips
		desc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
	}
	else
	{
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.MiscFlags = 0;
	}
	desc.CPUAccessFlags = 0;
	

	HRESULT hr = D3D11Device->CreateTexture2D(&desc, NULL/*SubresourceDataArray*/, &D3D11Texture2D);
	if (FAILED(hr))
		CHK(0);

	D3D11DeviceContext->UpdateSubresource(D3D11Texture2D, 0, NULL, Buffer, Width * PixelSize, 0);
	
	D3D11_SHADER_RESOURCE_VIEW_DESC ShaderResourceViewDesc;
	ZeroMemory(&ShaderResourceViewDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	ShaderResourceViewDesc.Format = GetFormatSRV(desc.Format);
	ShaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	ShaderResourceViewDesc.Texture2D.MipLevels = -1;
	ShaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	
	hr = D3D11Device->CreateShaderResourceView(D3D11Texture2D, &ShaderResourceViewDesc, &D3D11ShaderResouceView);
	if (FAILED(hr))
		CHK(0);

	if (MipLevels > 1)
		D3D11DeviceContext->GenerateMips(D3D11ShaderResouceView); 
}

LXTextureD3D11* LXTextureD3D11::CreateFromTexture(LXTexture* InTexture)
{
	LXTextureD3D11* TextureD3D11 = nullptr;

	if (InTexture->TextureSource == ETextureSource::TextureSourceBitmap)
	{
		if (LXBitmap* Bitmap = InTexture->GetBitmap(0))
		{
			DXGI_FORMAT Format = GetDXGIFormat(Bitmap->GetInternalFormat());
			uint MipLevels = 0;
			if (InTexture->GenerateMipMap())
				MipLevels = LXBitmap::GetNumMipLevels(Bitmap->GetWidth(), Bitmap->GetHeight());
			TextureD3D11 = new LXTextureD3D11(Bitmap->GetWidth(), Bitmap->GetHeight(), Format, Bitmap->GetPixels(), Bitmap->GetPixelSize(), MipLevels);
		}
		else
		{
			LogD(LXTextureD3D11, L"Texture does not contain a valid bitmap.");
		}
	}
	else if (InTexture->TextureSource == ETextureSource::TextureSourceMaterial)
	{
		DXGI_FORMAT Format = GetDXGIFormat(InTexture->GetInternalFormat());
		TextureD3D11 = new LXTextureD3D11(InTexture->GetWidth(), InTexture->GetHeight(), Format, false/*true*/);
	}
	else
	{
		LogD(LXTextureD3D11, L"Unknown texture source type."); 
	}

	return TextureD3D11;
}

LXTextureD3D11::~LXTextureD3D11()
{
	LX_COUNTSCOPEDEC(LXTextureD3D11)

	CHK(IsRenderThread())

	LX_SAFE_RELEASE(D3D11Texture2D)
	LX_SAFE_RELEASE(D3D11ShaderResouceView)
	LX_SAFE_RELEASE(D3D11RenderTargetView);
}

ID3D11RenderTargetView* LXTextureD3D11::GetRenderTargetView() const
{
	if (D3D11RenderTargetView)
		return D3D11RenderTargetView;

	// Create RenderTargetView
	DXGI_FORMAT FormatRTV;
	DXGI_FORMAT FormatSRV;
	LXTextureD3D11::GetFormats(_Format, FormatRTV, FormatSRV);

	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	renderTargetViewDesc.Format = FormatRTV;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;

	LXDirectX11* DirectX11 = LXDirectX11::GetCurrentDirectX11();
	HRESULT hr = DirectX11->_D3D11Device->CreateRenderTargetView(D3D11Texture2D, &renderTargetViewDesc, &D3D11RenderTargetView);
	if (FAILED(hr))
		CHK(0);

	return D3D11RenderTargetView;
}



void LXTextureD3D11::Create(DXGI_FORMAT Format, uint Width, uint Height, bool bSupportAutoMipmap)
{
	ID3D11Device* D3D11Device = LXDirectX11::GetCurrentDevice();
 
	D3D11_TEXTURE2D_DESC desc = { 0 };
	desc.Width = Width;
	desc.Height = Height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = (DXGI_FORMAT)Format;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	if ((Format == DXGI_FORMAT_R32_TYPELESS) || (Format == DXGI_FORMAT_R24G8_TYPELESS))
	{
		desc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
		desc.MiscFlags = 0;
	}
	else
	{
		desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		if (bSupportAutoMipmap)
		{
			desc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
			desc.MipLevels = 0;
		}
	}
	desc.CPUAccessFlags = 0;
	

	HRESULT hr =  D3D11Device->CreateTexture2D(&desc, NULL, &D3D11Texture2D);
	if (FAILED(hr))
		CHK(0);

	_Format = Format;

	//
	// Create the Shader Resource View
	//

	{
// 		D3D11_SHADER_RESOURCE_VIEW_DESC ShaderResourceViewDesc;
// 		ZeroMemory(&ShaderResourceViewDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
// 		ShaderResourceViewDesc.Format = desc.Format;
// 		ShaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
// 		ShaderResourceViewDesc.Texture2D.MipLevels = -1;// desc.MipLevels;
// 		ShaderResourceViewDesc.Texture2D.MostDetailedMip = 0;//desc.MipLevels - 1;
// 
// 		hr = D3D11Device->CreateShaderResourceView(D3D11Texture2D, &ShaderResourceViewDesc, &D3D11ShaderResouceView);
// 		if (FAILED(hr))
// 			CHK(0);

		DXGI_FORMAT FormatRTV;
		DXGI_FORMAT FormatSRV;
		GetFormats((DXGI_FORMAT)_Format, FormatRTV, FormatSRV);

		D3D11_TEXTURE2D_DESC TextureDesc;
		D3D11Texture2D->GetDesc(&TextureDesc);
		
		D3D11_SHADER_RESOURCE_VIEW_DESC ShaderResourceViewDesc;
		ZeroMemory(&ShaderResourceViewDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
		ShaderResourceViewDesc.Format = FormatSRV;
		ShaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		if (bSupportAutoMipmap)
			ShaderResourceViewDesc.Texture2D.MipLevels = -1;// TextureDesc.MipLevels;
		else
			ShaderResourceViewDesc.Texture2D.MipLevels = 1;// TextureDesc.MipLevels;
		ShaderResourceViewDesc.Texture2D.MostDetailedMip = 0;// TextureDesc.MipLevels - 1;

		hr = D3D11Device->CreateShaderResourceView(D3D11Texture2D, &ShaderResourceViewDesc, &D3D11ShaderResouceView);
		if (FAILED(hr))
			CHK(0);
	}
}

void LXTextureD3D11::CreateForCapture2(ID3D11Texture2D*SrcResource)
{
	ID3D11Device* D3D11Device = LXDirectX11::GetCurrentDevice();

	D3D11_TEXTURE2D_DESC desc;
	SrcResource->GetDesc(&desc);

	desc.Usage = D3D11_USAGE_STAGING;
	desc.BindFlags = 0;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

	HRESULT hr = D3D11Device->CreateTexture2D(&desc, NULL, &D3D11Texture2D);
	if (FAILED(hr))
		CHK(0);

	_Format = desc.Format;
}

void LXTextureD3D11::ReadToBitmap(LXRenderCommandList* RCL, LXBitmap* Bitmap)
{
	// Check if bitmap matches (Size and format)
// 	RCL->Map(D3D11Texture2D);
// 	RCL->Unmap(D3D11Texture2D);
}

DXGI_FORMAT LXTextureD3D11::GetDXGIFormat(ETextureFormat TextureFormat)
{
	switch (TextureFormat)
	{
	case ETextureFormat::LX_R16_USHORT: return DXGI_FORMAT_R16_UNORM; break;
	case ETextureFormat::LX_R16G16_USHORT: return DXGI_FORMAT_R16G16_UNORM; break;
	case ETextureFormat::LX_R16G16_FLOAT: return DXGI_FORMAT_R16G16_FLOAT; break;
	case ETextureFormat::LX_R32G32_FLOAT: return DXGI_FORMAT_R32G32B32_FLOAT; break;
	case ETextureFormat::LX_RGBA8: return DXGI_FORMAT_B8G8R8A8_UNORM;  break;

	case ETextureFormat::LX_RGB8: return DXGI_FORMAT_BC1_TYPELESS;  break;

	case ETextureFormat::LX_RGB32F: return DXGI_FORMAT_R32G32B32_FLOAT; break;
	case ETextureFormat::LX_RGBA32F: return DXGI_FORMAT_R32G32B32A32_FLOAT; break;
	default: CHK(0 && "Unknown bitmap format"); return DXGI_FORMAT_UNKNOWN; break;
	}
}


DXGI_FORMAT LXTextureD3D11::GetFormatSRV(DXGI_FORMAT TextureFormatDXGI)
{
	switch (TextureFormatDXGI)
	{
	case DXGI_FORMAT_BC1_TYPELESS: return DXGI_FORMAT_BC1_UNORM_SRGB; break;
	case DXGI_FORMAT_R32G32B32_FLOAT: return DXGI_FORMAT_R32G32B32_FLOAT; break;
	case DXGI_FORMAT_B8G8R8A8_UNORM: return DXGI_FORMAT_B8G8R8A8_UNORM; break;
	case DXGI_FORMAT_R32G32B32A32_FLOAT: return DXGI_FORMAT_R32G32B32A32_FLOAT; break;
	case DXGI_FORMAT_R16_UNORM: return DXGI_FORMAT_R16_UNORM; break;
	case DXGI_FORMAT_R32G32B32A32_UINT: return DXGI_FORMAT_R32G32B32A32_UINT; break;
	default: CHK(0); return DXGI_FORMAT_UNKNOWN;
	}
}

void LXTextureD3D11::GetFormats(DXGI_FORMAT TextureFormatDXGI, DXGI_FORMAT& OutFormatRTV, DXGI_FORMAT& OutFormatSRV)
{
	switch (TextureFormatDXGI)
	{
	case DXGI_FORMAT_BC1_TYPELESS:
	{
		OutFormatRTV = DXGI_FORMAT_BC1_UNORM_SRGB;
		OutFormatSRV = DXGI_FORMAT_BC1_UNORM_SRGB;
	}
	break;

		// Color HDR

		//  	case DXGI_FORMAT_R32G32B32A32_FLOAT:
		//  	{
		//  		OutFormatRTV = DXGI_FORMAT_R32G32B32A32_FLOAT;
		//  		OutFormatSRV = DXGI_FORMAT_R32G32B32A32_FLOAT;
		//  	}
		//  	break;

	case DXGI_FORMAT_R16G16B16A16_FLOAT: // Ok
	{
		OutFormatRTV = DXGI_FORMAT_R16G16B16A16_FLOAT;
		OutFormatSRV = DXGI_FORMAT_R16G16B16A16_FLOAT;
	}
	break;

	// Color

	case DXGI_FORMAT_B8G8R8A8_UNORM:
	case DXGI_FORMAT_B8G8R8A8_TYPELESS: // Ok
	{
		OutFormatRTV = DXGI_FORMAT_B8G8R8A8_UNORM; // DXGI_FORMAT_B8G8R8A8_UNORM_SRGB (For Albedo)
		OutFormatSRV = DXGI_FORMAT_B8G8R8A8_UNORM; // DXGI_FORMAT_B8G8R8A8_UNORM_SRGB (For Albedo)
	}
	break;

	// 	case DXGI_FORMAT_R8G8B8A8_UNORM: 
	// 	{
	// 		OutFormatRTV = ; 
	// 		GetFormatSRV = ; 
	// 	}
	// 	break;

	// Normal

	case DXGI_FORMAT_R10G10B10A2_UNORM: // Ok
	{
		OutFormatRTV = DXGI_FORMAT_R10G10B10A2_UNORM;
		OutFormatSRV = DXGI_FORMAT_R10G10B10A2_UNORM;
	}
	break;

	case DXGI_FORMAT_R11G11B10_FLOAT: // Ok
	{
		OutFormatRTV = DXGI_FORMAT_R11G11B10_FLOAT; 
		OutFormatSRV = DXGI_FORMAT_R11G11B10_FLOAT;
	}
	break;

	case DXGI_FORMAT_R32G32B32A32_FLOAT: // ?
	{
		OutFormatRTV = DXGI_FORMAT_R32G32B32A32_FLOAT;
		OutFormatSRV = DXGI_FORMAT_R32G32B32A32_FLOAT;
	}
	break;



	// Depth

	// 	case DXGI_FORMAT_R32_TYPELESS:
	// 	{
	// 		OutFormatRTV = DXGI_FORMAT_D32_FLOAT; // DSV
	// 		OutFormatSRV = DXGI_FORMAT_R32_FLOAT; 
	// 	}
	// 	break;

	case DXGI_FORMAT_R24G8_TYPELESS: // Ok
	{
		OutFormatRTV = DXGI_FORMAT_D24_UNORM_S8_UINT;
		OutFormatSRV = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	}
	break;

	case  DXGI_FORMAT_R16_UNORM: // Ok
	{
		OutFormatRTV = DXGI_FORMAT_R16_UNORM;
		OutFormatSRV = DXGI_FORMAT_R16_UNORM;
	}
	break;

	case  DXGI_FORMAT_R16G16_UNORM: // Ok
	{
		OutFormatRTV = DXGI_FORMAT_R16G16_UNORM;
		OutFormatSRV = DXGI_FORMAT_R16G16_UNORM;
	}
	break;

	case  DXGI_FORMAT_R16G16_FLOAT: // Ok
	{
		OutFormatRTV = DXGI_FORMAT_R16G16_FLOAT;
		OutFormatSRV = DXGI_FORMAT_R16G16_FLOAT;
	}
	break;

	case  DXGI_FORMAT_R32G32_FLOAT: // ?
	{
		OutFormatRTV = DXGI_FORMAT_R32G32_FLOAT;
		OutFormatSRV = DXGI_FORMAT_R32G32_FLOAT;
	}
	break;

	default:
	{
		CHK(0);
		OutFormatRTV = DXGI_FORMAT_UNKNOWN;
		OutFormatSRV = DXGI_FORMAT_UNKNOWN;
	}
	break;
	}
}

void LXTextureD3D11::SwapDeviceObjects(LXTextureD3D11* a, LXTextureD3D11* b)
{
	CHK(IsRenderThread());

	CHK(a->_Format == b->_Format);
	
	ID3D11Texture2D* D3D11Texture2D = a->D3D11Texture2D;
	ID3D11ShaderResourceView* D3D11ShaderResouceView = a->D3D11ShaderResouceView;
	ID3D11RenderTargetView* D3D11RenderTargetView = a->D3D11RenderTargetView;

	a->D3D11Texture2D = b->D3D11Texture2D;
	a->D3D11ShaderResouceView = b->D3D11ShaderResouceView;
	a->D3D11RenderTargetView = b->D3D11RenderTargetView;

	b->D3D11Texture2D = D3D11Texture2D;
	b->D3D11ShaderResouceView = D3D11ShaderResouceView;
	b->D3D11RenderTargetView = D3D11RenderTargetView;
}
