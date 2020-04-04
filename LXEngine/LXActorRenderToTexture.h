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

	void SetMaterial(LXMaterial* Material);
	const LXMaterial* GetMaterial() const { return _material; }

	void SetTexture(LXTexture* texture);
	const LXTexture* GetTexture() const { return _texture; }
	
	void CopyDeviceToBitmap();
	
private:
	
	void MaterialChanged();

private:

	LXMaterial* _material = nullptr;
	LXTexture* _texture = nullptr;
};

