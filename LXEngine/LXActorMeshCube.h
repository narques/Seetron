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

class LXCORE_API LXActorMeshCube : public LXActorMesh
{

public:

	LXActorMeshCube(LXProject*);
	virtual ~LXActorMeshCube();

private:

	shared_ptr<LXMaterial> _Material;

};

