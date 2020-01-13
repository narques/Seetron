//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXActorMesh.h"

class LXTexture;

class LXActorRenderToTexture : public LXActorMesh
{

public:

	LXActorRenderToTexture(LXProject* project);
	virtual ~LXActorRenderToTexture();

	const LXTexture* GetTexture() const { return _texture; }

private:

	LXTexture* _texture = nullptr;

};

