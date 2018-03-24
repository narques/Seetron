//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once
#include "LXCommand.h"

class LXMesh;

class LXCommandModifyActorHierarchy : public LXCommand
{

public:

	LXCommandModifyActorHierarchy(LXActor* InParent, LXActor* InChild);
	virtual ~LXCommandModifyActorHierarchy();
	bool Do() override;
	bool Undo() override;

private:

	LXActor* _Parent = nullptr;
	LXActor* _Child = nullptr;
};

class LXCommandModifyMeshHierarchy : public LXCommand
{

public:

	LXCommandModifyMeshHierarchy(LXMesh* InParent, LXMesh* InChild);
	virtual ~LXCommandModifyMeshHierarchy();
	bool Do() override;
	bool Undo() override;

private:

	LXMesh* _Parent = nullptr;
	LXMesh* _Child = nullptr;
};
