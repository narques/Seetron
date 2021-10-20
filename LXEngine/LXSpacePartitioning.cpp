//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "LXSpacePartitioning.h"
#include "LXTraverserCallBack.h"
#include "LXEngine.h"

LXSpacePartitioning::LXSpacePartitioning()
{
}

LXSpacePartitioning::~LXSpacePartitioning()
{
}

void LXSpacePartitioning::BuildOctree()
{
	LXTraverserLambda TCB(GetScene());
	TCB.SetLamndaOnPrimitive([](LXActorMesh* ActorMesh, LXWorldPrimitive* WorldPrimitive)
	{
	});

	TCB.Apply();

}

void LXSpacePartitioning::BuildBSP()
{

}

void LXSpacePartitioning::BuildBVH()
{
	// Top-down
}

LXSpacePartitioning* GetSpacePartitioning()
{
	static LXSpacePartitioning SP;
	return &SP;
}
