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

class LXRenderTarget : public LXObject
{

public:

	LXRenderTarget(uint Width, uint Height, DXGI_FORMAT Format);
	virtual ~LXRenderTarget();

	void CreateBuffers(uint Width, uint Height, DXGI_FORMAT Format);
	void DeleteBuffers();

	LXTextureD3D11*  _TextureD3D11 = nullptr;
	LXRenderTargetViewD3D11* _RenderTargetViewD3D11 = nullptr;
};

