//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once
#include "LXActorMesh.h"

class LXActorTree : public LXActorMesh
{

public:

	LXActorTree();
	virtual ~LXActorTree();

private:

	void OnPropertyChanged(LXProperty* pProperty) override;

	float TrunkHeight = 1000.f;	// TODO M2CM(10.f);
	float TrunkRadius = 50.f;	// TODO M2CM(1.f);
	int Slices = 8;
	int Stacks = 8;

	// Refs
	LXPrimitive* TrunkPrimitive;

};

class LXTreeElement : public LXActorMesh
{

public:

	LXTreeElement();
	virtual ~LXTreeElement();

};


