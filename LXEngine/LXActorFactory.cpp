//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "LXActorFactory.h"
#include "LXActorMeshSphere.h"
#include "LXActorMeshCube.h"
#include "LXActorMeshPlane.h"
#include "LXActorCamera.h"
#include "LXActorLight.h"
#include "LXActorRenderToTexture.h"
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
}, []()
{
	return GetCore().GetProject() != nullptr;
});

LXConsoleCommandNoArg CCCreateCube(L"Create.Cube", []()
{
	if (LXProject* Project = GetProject())
	{
		LXActorMesh* Actor = new LXActorMeshCube(Project);
		Project->GetScene()->AddChild(Actor);
	}
}, []()
{
	return GetCore().GetProject() != nullptr;
});

LXConsoleCommandNoArg CCCreateCone(L"Create.Cone", []()
{
	if (LXProject* Project = GetProject())
	{
		LXActor* Actor = new LXActorMeshCone();
		Project->GetScene()->AddChild(Actor);
	}
}, []()
{
	return GetCore().GetProject() != nullptr;
});

LXConsoleCommandNoArg CCCreateSphere(L"Create.Sphere", []()
{
	if (LXProject* Project = GetProject())
	{
		LXActorMesh* Actor = new LXActorMeshSphere(Project);
		Project->GetScene()->AddChild(Actor);
	}
}, []()
{
	return GetCore().GetProject() != nullptr;
});

LXConsoleCommandNoArg CCCreatePlane(L"Create.Plane", []()
{
	if (LXProject* Project = GetProject())
	{
		LXActor* Actor = new LXActorMeshPlane(Project);
		Project->GetScene()->AddChild(Actor);
	}
}, []()
{
	return GetCore().GetProject() != nullptr;
});

LXConsoleCommandNoArg CCCreateLight(L"Create.Light", []()
{
	if (LXProject* Project = GetProject())
	{
		LXActor* Actor = new LXActorLight(Project);
		Project->GetScene()->AddChild(Actor);
	}
}, []()
{
	return GetCore().GetProject() != nullptr;
});

LXConsoleCommandNoArg CCCreateSceneCapture(L"Create.SceneCapture", []()
{
	if (LXProject* Project = GetProject())
	{
		LXActor* Actor = new LXActorSceneCapture();
		Project->GetScene()->AddChild(Actor);
	}
}, []()
{
	return GetCore().GetProject() != nullptr;
});

LXConsoleCommandNoArg CCCreateRenderToTexture(L"Create.RenderToTexture", []()
{
	if (LXProject* Project = GetProject())
	{
		LXActor* Actor = new LXActorRenderToTexture(Project);
		Project->GetScene()->AddChild(Actor);
	}
}, []()
{
	return GetCore().GetProject() != nullptr;
});

LXConsoleCommandNoArg CCCreateTerrain(L"Create.Terrain", []()
{
	if (LXProject* Project = GetProject())
	{
		LXTerrain* Actor = new LXTerrain(Project);
		Project->GetScene()->AddChild(Actor);
	}
}, []()
{
	return GetCore().GetProject() != nullptr;
});

LXConsoleCommandNoArg CCCreateForest(L"Create.Forest", []()
{
	if (LXProject* Project = GetProject())
	{
		LXActor* Actor = new LXActorForest();
		Project->GetScene()->AddChild(Actor);
	}
}, []()
{
	return GetCore().GetProject() != nullptr;
});

LXActorFactory::LXActorFactory()
{
}

LXActorFactory::~LXActorFactory()
{
}

LXActor* LXActorFactory::CreateActor(const LXString& className)
{
	LXActor* actor = nullptr;
	LXProject* project = GetProject();

	if (className == L"LXActor")
	{
		actor = new LXActor(project);
	}
	else if (className == L"LXActorMesh")
	{
		actor = new LXActorMesh();
	}
	else if (className == L"LXActorMeshSphere")
	{
		actor = new LXActorMeshSphere(project);
	}
	else if (className == L"LXActorMeshCube")
	{
		actor = new LXActorMeshCube(project);
	}
	else if (className == L"LXActorMeshCylinder")
	{
		actor = new LXActorMeshCylinder(project);
	}
	else if (className == L"LXActorMeshPlane")
	{
		actor = new LXActorMeshPlane(project);
	}
	else if (className == L"LXActorCamera")
	{
		actor = new LXActorCamera(project);
	}
	else if (className == L"LXActorLight")
	{
		actor = new LXActorLight(project);
	}
	else if (className == L"LXTerrain")
	{
		actor = new LXTerrain(project);
	}
	else if (className == L"LXActorForest")
	{
		actor = new LXActorForest();
	}
	else if (className == L"LXActorSceneCapture")
	{
		actor = new LXActorSceneCapture();
		project->SetSceneCapture((LXActorSceneCapture*)actor);
	}
	else if (className == L"LXActorRenderToTexture")
	{
		actor = new LXActorRenderToTexture(project);
	}
	else
	{
		LogE(LXActorFactory, L"Unable to create object of class %s", className.GetBuffer());
		CHK(0);
	}
	
	return actor;
}
