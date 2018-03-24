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
struct ID3D11RenderTargetView;
struct ID3D11DepthStencilView;

class LXRenderTargetViewD3D11 : public LXObject
{

public:

	LXRenderTargetViewD3D11(LXTextureD3D11* Texture);
	virtual ~LXRenderTargetViewD3D11();


	ID3D11RenderTargetView* D3D11RenderTargetView;


};

class LXDepthStencilViewD3D11 : public LXObject
{

public:

	LXDepthStencilViewD3D11(LXTextureD3D11* Texture);
	virtual ~LXDepthStencilViewD3D11();


	ID3D11DepthStencilView* D3D11DepthStencilView;


};

