//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXCore/LXObject.h"

class LXTextureD3D11; 
class LXTexture;

class LXTextureManager : public LXObject
{

public:

	LXTextureManager();
	virtual ~LXTextureManager();

	const std::shared_ptr<LXTextureD3D11>& GetTexture(LXTexture* Texture);

private:

	std::shared_ptr<LXTextureD3D11> CreateTexture(const LXTexture* Texture);
	
private:

	std::map<const LXTexture*, std::shared_ptr<LXTextureD3D11>> _MapTextureD3D11;
};

