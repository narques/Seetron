
//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once
#include "LXCore/LXObject.h"

class LXRenderCluster;

class LXRenderBatching : public LXObject
{

public:

	LXRenderBatching();
	virtual ~LXRenderBatching();

	// Updates the given CustomRenderList adding InstancedRenderClusters.
	void Do(std::list<LXRenderCluster*>& renderClusters);

	void Clear();

private:

	typedef std::pair<const class LXPrimitiveD3D11*, const class LXMaterialBase*> Key;
	typedef std::map<Key, LXRenderCluster*> Map;
	Map _batchedRenderCusterOpaques;
};


