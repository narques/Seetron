//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXObject.h"
#include "LXDirectX11.h"

class LXTextureSampler;

class LXSamplerStateD3D11 : public LXObject
{
public:

	LXSamplerStateD3D11();
	LXSamplerStateD3D11(D3D11_FILTER filter, D3D11_TEXTURE_ADDRESS_MODE addressU, D3D11_TEXTURE_ADDRESS_MODE addressV, D3D11_TEXTURE_ADDRESS_MODE addressW);
	virtual ~LXSamplerStateD3D11();

	static LXSamplerStateD3D11* CreateFromSampler(LXTextureSampler* textureSampler);
	
private:

	void Create(D3D11_FILTER filter, D3D11_TEXTURE_ADDRESS_MODE addressU, D3D11_TEXTURE_ADDRESS_MODE addressV, D3D11_TEXTURE_ADDRESS_MODE addressW);


public:

	ID3D11SamplerState* D3D11SamplerState = nullptr;

};

