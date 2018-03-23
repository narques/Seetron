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

class LXCORE_API LXActorMeshSphere : public LXActorMesh
{

public:

	LXActorMeshSphere(LXProject*);
	virtual ~LXActorMeshSphere();

private:

	void OnPropertyChanged(LXProperty* Property) override;

private:

	LXMaterial* _Material = nullptr;
	int Slices = 64;
	int Stacks = 64;
	
};

