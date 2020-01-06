//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXRenderClusterType.h"

class LXActor;
class LXActorMesh;
class LXMaterial;
class LXMaterialD3D11;
class LXPrimitive;
class LXPrimitiveD3D11;
class LXRenderCluster;
class LXRendererUpdateMatrix;
class LXPrimitiveD3D11;
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
	void AddActor(LXActor* Actor, LXFlagsRenderClusterRole renderClusterRole);
	void UpdateActor(LXActor* Actor, LXFlagsRenderClusterRole renderClusterRole);
	void RemoveActor(LXActor* Actor, LXFlagsRenderClusterRole renderClusterRole);

	// PrimitiveInstance
	void UpdateMatrix(const LXRendererUpdateMatrix& RendererUpdateMatrix);
	
	// Misc
	const map<LXActor*, list<LXRenderCluster*>>& GetActors() { return ActorRenderCluster; }
	
private:

	shared_ptr<LXPrimitiveD3D11>& GetPrimitiveD3D11(LXPrimitive* Primitive, const ArrayVec3f* ArrayInstancePosition = nullptr);
	LXRenderCluster* CreateRenderCluster(LXActorMesh* Actor, LXWorldPrimitive* worldPrimitive, const LXMatrix& MatrixWCS, const LXBBox& BBoxWorld, LXPrimitive* Primitive, LXMaterial* Material);

	// Remove the RenderCluster from the Rendering (ListRendersClusters)
	// Plus the additional helper maps
	// The RenderCluster is not deleted.

public:

	list<LXRenderCluster*> ListRenderClusters;
		
private:

	//
	// Helper maps to simplify data access
	//

	map<LXActor*, list<LXRenderCluster*>> ActorRenderCluster;
	multimap<LXWorldPrimitive*, LXRenderCluster*> PrimitiveInstanceRenderClusters;

	//
	// D3D11 Shared Resources
	//

	map<pair<LXPrimitive*, uint>, shared_ptr<LXPrimitiveD3D11>> MapPrimitiveD3D11;
};

