//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXRenderClusterType.h"

class LXMaterialBase;
class LXPrimitive;
class LXRenderCluster;
class LXRenderData;
class LXWorldPrimitive;

class LXRenderClusterManager
{
	friend class LXRenderCluster;

public:

	LXRenderClusterManager();
	~LXRenderClusterManager();

	void Empty();
	void Tick();

	// Actor
	void AddActor(LXRenderData* renderData, LXFlagsRenderClusterRole renderClusterRole);
	void UpdateActor(LXRenderData* renderData, LXFlagsRenderClusterRole renderClusterRole);
	void RemoveActor(LXRenderData* renderData, LXFlagsRenderClusterRole renderClusterRole);

	// Misc
	const std::map<LXRenderData*, std::list<LXRenderCluster*>>& GetActors() { return ActorRenderCluster; }
	
private:

	LXRenderCluster* CreateRenderCluster(LXRenderData* renderData, const std::vector<LXWorldPrimitive*>& worldPrimitiveLODs, const LXMatrix& MatrixWCS, const LXMatrix& matrix, const LXBBox& BBoxWorld, const std::vector<LXPrimitive*>& primitiveLODs, const LXMaterialBase* Material);

	// Remove the RenderCluster from the Rendering (ListRendersClusters)
	// Plus the additional helper maps
	// The RenderCluster is not deleted.

public:

	std::list<LXRenderCluster*> ListRenderClusters;
		
private:

	//
	// Helper maps to simplify data access
	//

	std::map<LXRenderData*, std::list<LXRenderCluster*>> ActorRenderCluster;
};

