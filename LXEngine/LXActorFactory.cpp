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

LXConsoleCommandNoArg CCCreateCylinder(L"Create.Cylinder", []()
{
	if (LXProject* Project = GetProject())
	{
		LXActorMesh* Actor = new LXActorMeshCylinder(Project);
		Project->GetScene()->AddChild(Actor);
	}
});

LXConsoleCommandNoArg CCCreateCube(L"Create.Cube", []()
{
	if (LXProject* Project = GetProject())
	{
		LXActorMesh* Actor = new LXActorMeshCube(Project);
		Project->GetScene()->AddChild(Actor);
	}
});

LXConsoleCommandNoArg CCCreateCone(L"Create.Cone", []()
{
	if (LXProject* Project = GetProject())
	{
		LXActor* Actor = new LXActorMeshCone();
		Project->GetScene()->AddChild(Actor);
	}
});

LXConsoleCommandNoArg CCCreateSphere(L"Create.Sphere", []()
{
	if (LXProject* Project = GetProject())
	{
		LXActorMesh* Actor = new LXActorMeshSphere(Project);
		Project->GetScene()->AddChild(Actor);
	}
});

LXConsoleCommandNoArg CCCreatePlane(L"Create.Plane", []()
{
	if (LXProject* Project = GetProject())
	{
		LXActor* Actor = new LXActorMeshPlane(Project);
		Project->GetScene()->AddChild(Actor);
	}
});

LXConsoleCommandNoArg CCCreateLight(L"Create.Light", []()
{
	if (LXProject* Project = GetProject())
	{
		LXActor* Actor = new LXActorLight(Project);
		Project->GetScene()->AddChild(Actor);
	}
});

LXConsoleCommandNoArg CCCreateSceneCapture(L"Create.SceneCapture", []()
{
	if (LXProject* Project = GetProject())
	{
		LXActor* Actor = new LXActorSceneCapture();
		Project->GetScene()->AddChild(Actor);
	}
});

LXConsoleCommandNoArg CCCreateTerrain(L"Create.Terrain", []()
{
	if (LXProject* Project = GetProject())
	{
		LXTerrain* Actor = new LXTerrain(Project);
		Project->GetScene()->AddChild(Actor);
	}
});

LXConsoleCommandNoArg CCCreateForest(L"Create.Forest", []()
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
	else if (ClassName == L"LXActorMeshCylinder")
	{
		Actor = new LXActorMeshCylinder(Project);
	}
	else if (ClassName == L"LXActorMeshPlane")
	{
		Actor = new LXActorMeshPlane(Project);
	}
	else if (ClassName == L"LXActorCamera")
	{
		Actor = new LXActorCamera(Project);
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
		LogE(LXActorFactory, L"Unable to create object of class %s", ClassName.GetBuffer());
		CHK(0);
	}
	
	return Actor;
}
