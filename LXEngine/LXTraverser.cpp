//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "StdAfx.h"
#include "LXTraverser.h"
#include "LXScene.h"
#include "LXActor.h"
#include "LXActor.h"
#include "LXActor.h"
#include "LXActorMesh.h"
#include "LXPrimitive.h"
#include "LXPrimitive.h"
#include "LXPerformance.h"
#include "LXCore.h"
#include "LXMemory.h" // --- Must be the last included ---

LXTraverser::LXTraverser(void)
{
}

LXTraverser::~LXTraverser(void)
{
}

void LXTraverser::Apply( )
{
	if (_Scene)
	{
		OnActor(_Scene);
	}
}

void LXTraverser::OnActor(LXActor* pActor)
{
	if (!pActor)
		return;

	for(LXActor* Actor : pActor->GetChildren())
	{
		OnActor(Actor);
	}

	if (pActor->GetCID() & LX_NODETYPE_MESH)
	{
		OnMesh((LXActorMesh*)pActor);
	}
}

void LXTraverser::OnMesh(LXActorMesh* pMesh)
{
	const TWorldPrimitives& WorldPrimitive = pMesh->GetAllPrimitives();
	for (const LXWorldPrimitive* It : WorldPrimitive)
	{
		OnPrimitive(pMesh, const_cast<LXWorldPrimitive*>(It));
	}
}
