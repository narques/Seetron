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

class LXENGINE_API LXActorMeshSphere : public LXActorMesh
{

public:

	LXActorMeshSphere(LXProject*);
	virtual ~LXActorMeshSphere();

private:

	void OnPropertyChanged(LXProperty* Property) override;

private:

	std::shared_ptr<LXMaterial> _material;
	int Slices = 64;
	int Stacks = 64;
	
};

//------------------------------------------------------------------------------------------------------

class LXENGINE_API LXActorMeshCylinder : public LXActorMesh
{

public:

	LXActorMeshCylinder(LXProject*);
	virtual ~LXActorMeshCylinder();

private:

	void OnPropertyChanged(LXProperty* Property) override;

private:

	std::shared_ptr<LXMaterial> _material;
	int Slices = 64;
	int Stacks = 64;

};

//------------------------------------------------------------------------------------------------------

class LXENGINE_API LXActorMeshCone : public LXActorMesh
{

public:

	LXActorMeshCone();
	virtual ~LXActorMeshCone();

private:

	void OnPropertyChanged(LXProperty* Property) override;

private:

	std::shared_ptr<LXMaterial> _material;
	int Slices = 64;
	int Stacks = 64;

};

