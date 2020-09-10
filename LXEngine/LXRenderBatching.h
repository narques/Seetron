
//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once
#include "LXObject.h"

class LXRenderCluster;

class LXRenderBatching : public LXObject
{

public:

	LXRenderBatching();
	virtual ~LXRenderBatching();

	// Updates the given CustomRenderList adding InstancedRenderClusters.
	void Do(list<LXRenderCluster*>& renderClusters);

	void Clear();

private:

	typedef pair<const class LXPrimitiveD3D11*, const class LXMaterialBase*> Key;
	typedef map<Key, LXRenderCluster*> Map;
	Map _batchedRenderCusterOpaques;
};


