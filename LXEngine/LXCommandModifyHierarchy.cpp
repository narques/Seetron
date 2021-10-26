//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "pch.h"
#include "LXCommandModifyHierarchy.h"
#include "LXActor.h"
#include "LXMesh.h"
#include "LXLogger.h"

LXCommandModifyActorHierarchy::LXCommandModifyActorHierarchy(LXActor* InParent, LXActor* InChild):
	_Parent(InParent), 
	_Child(InChild)
{
}

LXCommandModifyActorHierarchy::~LXCommandModifyActorHierarchy()
{
}

bool LXCommandModifyActorHierarchy::Do()
{
	if (!_Child || !_Parent || _Parent == _Child)
	{
		LogW(CommandModifyHierarchie, L"Unknown error");
		return false;
	}

	// Update matrix to avoid visual transformation
	LXMatrix WCS = _Child->GetMatrixWCS();

	// Detach from the current parent
	LXActor* PreviousParent = _Child->GetParent();
	PreviousParent->RemoveChild(_Child);

	// Attach to new parent
	_Parent->AddChild(_Child);

	// Compute the local matrix based on the world matrix
	_Child->SetMatrixWCS(WCS, true);
	
	return true;
}

bool LXCommandModifyActorHierarchy::Undo()
{
	CHK(0);
	return true;
}

//------------------------------------------------------------------------------------------------------

LXCommandModifyMeshHierarchy::LXCommandModifyMeshHierarchy(LXMesh* InParent, LXMesh* InChild) :
	_Parent(InParent),
	_Child(InChild)
{
}

LXCommandModifyMeshHierarchy::~LXCommandModifyMeshHierarchy()
{
}

bool LXCommandModifyMeshHierarchy::Do()
{
	if (!_Child || !_Parent || _Parent == _Child)
	{
		LogW(CommandModifyHierarchie, L"Unknown error");
		return false;
	}

	CHK(0);

	/*
	// Update matrix to avoid visual transformation
	LXMatrix WCS = Child->GetMatrixWCS();

	// Detach from the current parent
	LXMesh* PreviousParent = Child->GetParent();
	PreviousParent->RemoveChild(Child);

	// Attach to new parent
	Parent->AddChild(Child);

	// Compute the local matrix based on the world matrix
	Child->SetMatrixWCS(WCS, true);
	*/

	return true;
}

bool LXCommandModifyMeshHierarchy::Undo()
{
	CHK(0);
	return true;
}

