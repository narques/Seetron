//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXActor.h"

class LXTexture;

class LXActorSceneCapture : public LXActor
{

public:

	LXActorSceneCapture();
	virtual ~LXActorSceneCapture();

	float GetIntensity() const { return _Intensity; }
	const shared_ptr<LXTexture>& GetTexture() const { return _Texture; }
	
private:

	float _Intensity = 1.f;
	shared_ptr<LXTexture> _Texture = nullptr;
};

