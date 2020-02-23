//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXRenderClusterType.h"

class LXActorMesh;
class LXMaterial;
class LXPrimitive;
class LXPrimitiveD3D11;
class LXRenderCluster;
class LXRenderData;
class LXRendererUpdateMatrix;
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

	// PrimitiveInstance
	void UpdateMatrix(const LXRendererUpdateMatrix& RendererUpdateMatrix);
	
	// Misc
	const map<LXRenderData*, list<LXRenderCluster*>>& GetActors() { return ActorRenderCluster; }
	
private:

	shared_ptr<LXPrimitiveD3D11>& GetPrimitiveD3D11(LXPrimitive* Primitive, const ArrayVec3f* ArrayInstancePosition = nullptr);
	LXRenderCluster* CreateRenderCluster(LXRenderData* renderData, LXWorldPrimitive* worldPrimitive, const LXMatrix& MatrixWCS, const LXBBox& BBoxWorld, LXPrimitive* Primitive, LXMaterial* Material);

	// Remove the RenderCluster from the Rendering (ListRendersClusters)
	// Plus the additional helper maps
	// The RenderCluster is not deleted.

public:

	list<LXRenderCluster*> ListRenderClusters;
		
private:

	//
	// Helper maps to simplify data access
	//

	map<LXRenderData*, list<LXRenderCluster*>> ActorRenderCluster;
	multimap<LXWorldPrimitive*, LXRenderCluster*> PrimitiveInstanceRenderClusters;

	//
	// D3D11 Shared Resources
	//

	map<pair<LXPrimitive*, uint>, shared_ptr<LXPrimitiveD3D11>> MapPrimitiveD3D11;
};

