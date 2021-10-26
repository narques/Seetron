//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "pch.h"
#include "LXTraverser.h"

// Seetron
#include "LXActor.h"
#include "LXActorMesh.h"
#include "LXComponentMesh.h"
#include "LXPrimitive.h"
#include "LXScene.h"

LXTraverser::LXTraverser()
{
}

LXTraverser::~LXTraverser()
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

	for (LXComponent* component : pActor->GetComponents())
	{
		if (LXComponentMesh* componentMesh = dynamic_cast<LXComponentMesh*>(component))
		{
			OnMesh(componentMesh);
		}
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
		OnPrimitive(pMesh, nullptr, const_cast<LXWorldPrimitive*>(It));
	}
}

void LXTraverser::OnMesh(LXComponentMesh* componentMesh)
{
	const TWorldPrimitives& WorldPrimitive = componentMesh->GetAllPrimitives();
	for (const LXWorldPrimitive* It : WorldPrimitive)
	{
		OnPrimitive(nullptr, componentMesh, const_cast<LXWorldPrimitive*>(It));
	}
}
