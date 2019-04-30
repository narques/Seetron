//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma 

#include "LXRenderPass.h"

class LXActor;
class LXActorMesh;
class LXMaterial;
class LXMaterialD3D11;
class LXPrimitive;
class LXPrimitiveD3D11;
class LXRenderCluster;
class LXRendererUpdateMatrix;
class LXPrimitiveD3D11;
class LXShaderProgramD3D11;
class LXPrimitiveInstance;

class LXRenderClusterManager
{
	friend class LXRenderCluster;

public:

	LXRenderClusterManager();
	~LXRenderClusterManager();

	void Empty();
	void Tick();

	// Actor
	void AddActor(LXActor* Actor);
	//void MoveActor(LXActor* Actor);
	void UpdateActor(LXActor* Actor);
	void RemoveActor(LXActor* Actor);

	// PrimitiveInstance
	void UpdateMatrix(const LXRendererUpdateMatrix& RendererUpdateMatrix);
	
	// Material
	void UpdateMaterial(const LXMaterial* Material);
	void RebuildMaterial(const LXMaterial* Material);
	LXMaterialD3D11* GetMaterial(const LXMaterial* Material);

	// Misc
	const map<LXActor*, list<LXRenderCluster*>>& GetActors() { return ActorRenderCluster; }
	
private:

	LXMaterialD3D11* GetMaterialD3D11(const LXMaterial* Material);
	shared_ptr<LXPrimitiveD3D11>& GetPrimitiveD3D11(LXPrimitive* Primitive, const ArrayVec3f* ArrayInstancePosition = nullptr);
	bool GetMaterialAndShadersD3D11(LXRenderCluster* renderCluster, const LXMaterial* Material, const LXPrimitiveD3D11* PrimitiveD3D11);
	bool GetShadersD3D11(ERenderPass renderPass, const LXPrimitiveD3D11* primitiveD3D11, const LXMaterialD3D11* materialD3D11, LXShaderProgramD3D11* shaderProgram);
	LXRenderCluster* CreateRenderCluster(LXActorMesh* Actor, LXPrimitiveInstance* PrimitiveInstance, const LXMatrix& MatrixWCS, const LXBBox& BBoxWorld, LXPrimitive* Primitive, LXMaterial* Material);

	// Remove the RenderCluster from the Rendering (ListRendersClusters)
	// Plus the additional helper maps
	// The RenderCluster is not deleted.
	void RemoveRenderCluster(LXRenderCluster* RenderCluster);

public:

	list<LXRenderCluster*> ListRenderClusters;
		
private:

	//
	// Helper maps to simplify data access
	//

	map<LXActor*, list<LXRenderCluster*>> ActorRenderCluster;
	map<const LXMaterial*, list<LXRenderCluster*>> MaterialRenderClusters;
	map<LXPrimitiveInstance*, LXRenderCluster*> PrimitiveInstanceRenderClusters;

	//
	// D3D11 Shared Resources
	//

	map<const LXMaterial*, LXMaterialD3D11*> MapMaterialD3D11;
	map<pair<LXPrimitive*, uint>, shared_ptr<LXPrimitiveD3D11>> MapPrimitiveD3D11;
};

