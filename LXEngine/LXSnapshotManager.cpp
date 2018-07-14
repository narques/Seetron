//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "LXSnapshotManager.h"
#include "LXSnapshot.h"
#include "LXSmartObject.h"
#include "LXMSXMLNode.h"
#include "LXMemory.h" // --- Must be the last included ---

LXSnapshotManager::LXSnapshotManager(const LXProject* pDocument)
{
}

LXSnapshotManager::~LXSnapshotManager()
{
}

void LXSnapshotManager::GetObjects(ListObjects& listObject)
{
	for (auto It = _listSnapshots.begin(); It != _listSnapshots.end(); It++)
	{
		listObject.push_back(*It);
	}
}

bool LXSnapshotManager::OnSaveChild(const TSaveContext& saveContext) const
{
	for (auto It = _listSnapshots.begin(); It != _listSnapshots.end(); It++)
		(*It)->Save(saveContext);

	return true;
}

bool LXSnapshotManager::OnLoadChild(const TLoadContext& loadContext)
{
	if (loadContext.node.name() == L"LXSnapshot")
	{
		LXSnapshot* pSnapshot = new LXSnapshot();
		pSnapshot->Load(loadContext);
	}
	else
		CHK(0);

	return true;
}

void LXSnapshotManager::AddSnapshot(LXSnapshot* snapshot)
{
	LX_CHK_RET(snapshot);
	_listSnapshots.push_back(snapshot);
}

void LXSnapshotManager::RemoveSnapshot(LXSnapshot* snapshot)
{
	LX_CHK_RET(snapshot);
	_listSnapshots.remove(snapshot);
}