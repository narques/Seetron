//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXObject.h"

class LXTextureD3D11;
class LXRenderTargetViewD3D11;
class LXDepthStencilViewD3D11;

class LXRenderTarget : public LXObject
{

public:

	LXRenderTarget(uint width, uint height, DXGI_FORMAT format);
	virtual ~LXRenderTarget();

	void CreateBuffers(uint width, uint height, DXGI_FORMAT format, bool deleteExisting = false);
	void DeleteBuffers();

	LXTextureD3D11*  TextureD3D11 = nullptr;
	LXRenderTargetViewD3D11* RenderTargetViewD3D11 = nullptr;

	const uint Width;
	const uint Height;
};

class LXRenderTargetDepth : public LXObject
{

public:

	LXRenderTargetDepth(uint width, uint height, DXGI_FORMAT format);
	virtual ~LXRenderTargetDepth();

	void CreateBuffers(uint width, uint height, DXGI_FORMAT format);
	void DeleteBuffers();

	LXTextureD3D11*  TextureD3D11 = nullptr;
	LXDepthStencilViewD3D11* RenderTargetViewD3D11 = nullptr;
};

