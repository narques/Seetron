//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "LXRenderTarget.h"
#include "LXTextureD3D11.h"
#include "LXRenderTargetViewD3D11.h"
#include "LXMemory.h" // --- Must be the last included ---

LXRenderTarget::LXRenderTarget(uint width, uint height, DXGI_FORMAT Format):Width(width), Height(height)
{
	CreateBuffers(width, height, Format);
}

LXRenderTarget::~LXRenderTarget()
{
	DeleteBuffers();
}

void LXRenderTarget::CreateBuffers(uint width, uint height, DXGI_FORMAT format)
{
	CHK(TextureD3D11 == nullptr);
	CHK(RenderTargetViewD3D11 == nullptr);

	TextureD3D11 = new LXTextureD3D11(width, height, format);
	RenderTargetViewD3D11 = new LXRenderTargetViewD3D11(TextureD3D11);
}

void LXRenderTarget::DeleteBuffers()
{
	LX_SAFE_DELETE(RenderTargetViewD3D11); 
	LX_SAFE_DELETE(TextureD3D11);
}

LXRenderTargetDepth::LXRenderTargetDepth(uint width, uint height, DXGI_FORMAT format)
{
	CreateBuffers(width, height, format);
}

LXRenderTargetDepth::~LXRenderTargetDepth()
{
	DeleteBuffers();
}

void LXRenderTargetDepth::CreateBuffers(uint width, uint height, DXGI_FORMAT format)
{
	CHK(TextureD3D11 == nullptr);
	CHK(RenderTargetViewD3D11 == nullptr);

	TextureD3D11 = new LXTextureD3D11(width, height, format);
	RenderTargetViewD3D11 = new LXDepthStencilViewD3D11(TextureD3D11);
}

void LXRenderTargetDepth::DeleteBuffers()
{
	LX_SAFE_DELETE(RenderTargetViewD3D11);
	LX_SAFE_DELETE(TextureD3D11);
}
