//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "pch.h"
#include "LXSnapshot.h"
#include "LXEngine.h"
#include "LXSnapshotManager.h"
#include "LXProject.h"
#include "LXActorCamera.h"

LXSnapshot::LXSnapshot()
{
	LXProject* Project = GetEngine().GetProject();
	LX_CHK_RET(Project);
	Project->GetSnapshotManager().AddSnapshot(this);
	static int cpt = 0;
	SetName(L"Snapshot " + LXString::Number(cpt++));
	DefineProperties();
}

LXSnapshot::~LXSnapshot()
{
	LXProject* Project = GetEngine().GetProject();
	LX_CHK_RET(Project);
	Project->GetSnapshotManager().RemoveSnapshot(this);
}

void LXSnapshot::DefineProperties()
{
	DefinePropertyString(L"Description", LXPropertyID::DESCRIPTION, &_strDescription);
	//DefinePropertyListProperties(L"Properties", LXPropertyID::LISTPROPERTIES, &_listProperties);
}

void LXSnapshot::AddProperty(LXProperty* pProperty)
{
	LX_CHK_RET(pProperty);
	_listProperties.push_back(pProperty);
}

void LXSnapshot::CaptureCurrentCamera(LXProject* Project)
{
	LX_CHK_RET(Project);
	LXActorCamera* pCamera = Project->GetCamera();
	LX_CHK_RET(pCamera);
	const ListProperties& lp = pCamera->GetProperties();
	
	for (auto It = lp.begin(); It != lp.end(); It++)
	{
		if ((*It)->GetPersistent())
		{
			_listProperties.push_back(*It);
		}
	}
}
