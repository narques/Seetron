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

LXRenderTarget::LXRenderTarget(uint Width, uint Height, DXGI_FORMAT Format)
{
	//LogD(LXRenderTarget, L" Size: %ix%i", Width, Height);
	CreateBuffers(Width, Height, Format);
}

LXRenderTarget::~LXRenderTarget()
{
	DeleteBuffers();
}

void LXRenderTarget::CreateBuffers(uint Width, uint Height, DXGI_FORMAT Format)
{
	CHK(_TextureD3D11 == nullptr);
	CHK(_RenderTargetViewD3D11 == nullptr);

	_TextureD3D11 = new LXTextureD3D11(Width, Height, Format);
	_RenderTargetViewD3D11 = new LXRenderTargetViewD3D11(_TextureD3D11);
}

void LXRenderTarget::DeleteBuffers()
{
	LX_SAFE_DELETE(_RenderTargetViewD3D11); 
	LX_SAFE_DELETE(_TextureD3D11);
}

LXRenderTargetDepth::LXRenderTargetDepth(uint Width, uint Height, DXGI_FORMAT Format)
{
	//LogD(LXRenderTargetDepth, L" Size: %ix%i", Width, Height);
	CreateBuffers(Width, Height, Format);
}

LXRenderTargetDepth::~LXRenderTargetDepth()
{
	DeleteBuffers();
}

void LXRenderTargetDepth::CreateBuffers(uint Width, uint Height, DXGI_FORMAT Format)
{
	CHK(_TextureD3D11 == nullptr);
	CHK(_RenderTargetViewD3D11 == nullptr);

	_TextureD3D11 = new LXTextureD3D11(Width, Height, Format);
	_RenderTargetViewD3D11 = new LXDepthStencilViewD3D11(_TextureD3D11);
}

void LXRenderTargetDepth::DeleteBuffers()
{
	LX_SAFE_DELETE(_RenderTargetViewD3D11);
	LX_SAFE_DELETE(_TextureD3D11);
}
