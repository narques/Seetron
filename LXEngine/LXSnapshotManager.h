//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXManager.h"
#include "LXSnapshot.h"

class LXSnapshotManager : public LXManager2
{
	friend class LXSnapshot;

public:

	LXSnapshotManager(const LXProject* pDocument);
	virtual ~LXSnapshotManager();

	// Overridden from LXManager2
	virtual void GetObjects(ListObjects& listObject) override;
	virtual bool OnSaveChild(const TSaveContext& saveContext) const override;
	virtual bool OnLoadChild(const TLoadContext& loadContext) override;
	
private:

	void AddSnapshot(LXSnapshot* snapshot);
	void RemoveSnapshot(LXSnapshot* snapshot);

private:

	ListSnapshots _listSnapshots;
};

