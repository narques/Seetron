//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "LXActorForest.h"
#include "LXCore.h"
#include "LXScene.h"
#include "LXTerrain.h"
#include "LXActorTree.h"
#include "LXMath.h"
#include "LXConsoleManager.h"
#include "LXActorMeshCube.h"
#include "LXMemory.h" // --- Must be the last included ---

LXConsoleCommandNoArg CCGenerateForest(L"GenerateForest", []()
{
	if (LXScene* Scene = GetScene())
	{
		if (LXActorForest* ActorForest = dynamic_cast<LXActorForest*>(Scene->GetActor(L"Forest")))
		{
			ActorForest->Generate();
		}
	}
}, []()
{
	return GetCore().GetProject() != nullptr;
});

LXActorForest::LXActorForest()
{
	SetName("Forest");
	ArrayVec3f MyArrayPosition;
	DefineProperty(L"Test", &MyArrayPosition);
}

LXActorForest::~LXActorForest()
{
}

void LXActorForest::Generate()
{
	LXScene* Scene = GetScene();
	if (!Scene)
		return;
	
	LXActorTerrain* Terrain = dynamic_cast<LXActorTerrain*>(Scene->GetActor(L"Terrain"));
	if (!Terrain)
		return;

	// Delete previous content
	// CA CRASH
// 	ListActors& listActors = GetChildren();
// 	while(LXActor* Actor = listActors.back())
// 	{
// 		RemoveChild(Actor);
// 		Actor->MarkForDelete();
// 	}

	srand(59654);

	const int Instances = 1000;
	const float MaxDistance = M2CM(100.f);
	
// 	if (0)
// 	{
// 		for (int i = 0; i < Instances; i++)
// 		{
// 			//LXTree* Tree = new LXTree();
// 			LXActorMeshCube* Tree = new LXActorMeshCube(GetProject());
// 
// 			float x = RandomFloat() * MaxDistance;
// 			float y = RandomFloat() * MaxDistance;
// 			float z = Terrain->GetHeightAt(x, y);
// 
// 			AddChild(Tree);
// 			Tree->SetOrigin(vec3f(x, y, z));
// 		}
// 	}
	
	// MultiInstance
// 	if (1)
// 	{
// 		LXActorMeshCube* Tree = new LXActorMeshCube(GetProject());
// 		Tree->SetInstanceCount(Instances);
// 		float x, y, z;
// 		for (int i = 0; i < Instances; i++)
// 		{
// 			x = RandomFloat() * MaxDistance;
// 			y = RandomFloat() * MaxDistance;
// 			z = Terrain->GetHeightAt(x, y);
// 			Tree->SetInstancePosition(i, vec3f(x, y, z));
// 		}
// 		
// 		AddChild(Tree);
// 		//Tree->SetOrigin(vec3f(x, y, z));
// 	}

}

