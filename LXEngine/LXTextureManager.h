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
class LXTexture;

class LXTextureManager : public LXObject
{

public:

	LXTextureManager();
	virtual ~LXTextureManager();

	const shared_ptr<LXTextureD3D11>& GetTexture(LXTexture* Texture);

private:

	shared_ptr<LXTextureD3D11> CreateTexture(const LXTexture* Texture);
	
private:

	map<const LXTexture*, shared_ptr<LXTextureD3D11>> _MapTextureD3D11;
};

