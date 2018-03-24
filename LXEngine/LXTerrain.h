//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXActorMesh.h"

class LXProject;

class LXCORE_API LXTerrain : public LXActorMesh
{

public:

	LXTerrain(LXProject* Project);
	virtual ~LXTerrain(void);

	float GetHeightAt(float x, float y) const;

private:

	void Build();
	//float GetHeightScale() const;
	
private:

	int PatchCount = 64;
	uint QuadCount = 63;		// Patch size in Quad per line. we want 63+1 vertices per line : aligned with the displacement texture.
	float QuadSize = 100.f;		// 1 quad per meter
	
	LXMaterial* Material = nullptr;
};

