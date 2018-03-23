//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "LXActorFactory.h"
#include "LXActorMeshSphere.h"
#include "LXActorMeshCube.h"
#include "LXActorMeshPlane.h"
#include "LXActorCamera.h"
#include "LXActorLight.h"
#include "LXScene.h"
#include "LXTerrain.h"
#include "LXProject.h"
#include "LXActorSceneCapture.h"
#include "LXConsoleManager.h"
#include "LXActorForest.h"
#include "LXMemory.h" // --- Must be the last included ---

//
// Console commands
//

LXConsoleCommandNoArg CCCreateTerrain(L"CreateTerrain", []()
{
	GetCore().GetProject()->CreateTerrainActor();
});

LXConsoleCommandNoArg CCCreateActorSceneCapture(L"CreateActorSceneCapture", []()
{
	if (LXProject* Project = GetProject())
	{
		LXActor* Actor = new LXActorSceneCapture();
		Project->GetScene()->AddChild(Actor);
	}
});

LXConsoleCommandNoArg CCCreateActorForest(L"CreateActorForest", []()
{
	if (LXProject* Project = GetProject())
	{
		LXActor* Actor = new LXActorForest();
		Project->GetScene()->AddChild(Actor);
	}
});

LXActorFactory::LXActorFactory()
{
}

LXActorFactory::~LXActorFactory()
{
}

LXActor* LXActorFactory::CreateActor(const LXString& ClassName)
{
	LXActor* Actor = nullptr;
	LXProject* Project = GetProject();

	if (ClassName == L"LXActor")
	{
		Actor = new LXActor(Project);
	}
	else if (ClassName == L"LXActorMesh")
	{
		Actor = new LXActorMesh();
	}
	else if (ClassName == L"LXActorMeshSphere")
	{
		Actor = new LXActorMeshSphere(Project);
	}
	else if (ClassName == L"LXActorMeshCube")
	{
		Actor = new LXActorMeshCube(Project);
	}
	else if (ClassName == L"LXActorMeshPlane")
	{
		Actor = new LXActorMeshPlane(Project);
	}
	else if (ClassName == L"LXActorCamera")
	{
		Actor = new LXActorCamera(Project);
		Project->SetCamera((LXActorCamera*)Actor);
	}
	else if (ClassName == L"LXCamera")
	{
		Actor = new LXActorCamera(Project);
		Project->SetCamera((LXActorCamera*)Actor);
	}
	else if (ClassName == L"LXActorLight")
	{
		Actor = new LXActorLight(Project);
	}
	else if (ClassName == L"LXTerrain")
	{
		Actor = new LXTerrain(Project);
	}
	else if (ClassName == L"LXActorForest")
	{
		Actor = new LXActorForest();
	}
	else if (ClassName == L"LXActorSceneCapture")
	{
		Actor = new LXActorSceneCapture();
		Project->SetSceneCapture((LXActorSceneCapture*)Actor);
	}
	else
	{
		CHK(0);
	}
	
	return Actor;
}
