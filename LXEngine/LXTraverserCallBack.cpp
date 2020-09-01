//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "StdAfx.h"
#include "LXTraverserCallBack.h"
#include "LXScene.h"
#include "LXActorMesh.h"
#include "LXPrimitiveInstance.h"
#include "LXMemory.h" // --- Must be the last included ---

LXTraverserLambda::LXTraverserLambda(LXScene* pScene)
{
	_Scene = pScene;
	CHK(_Scene);
}

void LXTraverserLambda::OnActor(LXActor* pGroup)
{
	if (_lambdaOnGroup)
		_lambdaOnGroup(pGroup);

	LXTraverser::OnActor(pGroup);
}


void LXTraverserLambda::OnMesh(LXActorMesh* pMesh)
{
	if (_lambdaOnMesh)
		_lambdaOnMesh(pMesh);
	
	LXTraverser::OnMesh(pMesh);
}

void LXTraverserLambda::OnPrimitive(LXActorMesh* pMesh, LXComponentMesh* componentMesh, LXWorldPrimitive* WorldPrimitive)
{
	if (_lambdaOnPrimitive)
		_lambdaOnPrimitive(pMesh, WorldPrimitive);

	LXTraverser::OnPrimitive(pMesh, componentMesh, WorldPrimitive);
}
