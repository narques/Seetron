//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "LXActor.h"
#include "LXActorMesh.h"
#include "LXController.h"
#include "LXCore.h"
#include "LXLogger.h"
#include "LXPrimitive.h"
#include "LXPrimitiveD3D11.h"
#include "LXPrimitiveFactory.h"
#include "LXPrimitiveInstance.h"
#include "LXRenderCluster.h"
#include "LXRenderClusterManager.h"
#include "LXRenderer.h"
#include "LXShaderManager.h"
#include "LXMemory.h" // --- Must be the last included ---

LXRenderClusterManager::LXRenderClusterManager()
{
}

LXRenderClusterManager::~LXRenderClusterManager()
{
	Empty();
}

void LXRenderClusterManager::Empty()
{
	for (LXRenderCluster* RenderCluster : ListRenderClusters)
	{
		delete RenderCluster;
	}

	ListRenderClusters.clear();
	ActorRenderCluster.clear();
	PrimitiveInstanceRenderClusters.clear();
	MapPrimitiveD3D11.clear();
}

void LXRenderClusterManager::Tick()
{
	auto it = MapPrimitiveD3D11.begin();
	while (it != MapPrimitiveD3D11.end())
	{
		// Delete the PrimitiveD3D11 when referenced by the map only
		if (it->second.use_count() == 1)
		{
			auto toErase = it;
			++it;
			MapPrimitiveD3D11.erase(toErase);
		}
		else
		{
			++it;
		}
	}
}

void LXRenderClusterManager::AddActor(LXActor* Actor)
{
	CHK(ActorRenderCluster.find(Actor) == ActorRenderCluster.end());
	
	if (Actor->GetCID() & LX_NODETYPE_ANCHOR)
		return;

	if (Actor->GetCID() & LX_NODETYPE_CAMERA)
		return;

	if (LXActorMesh* ActorMesh = LXCast<LXActorMesh>(Actor))
	{
		const TWorldPrimitives& WorldPrimitives = ActorMesh->GetAllPrimitives(true);

		for(auto &It:WorldPrimitives)
		{
			LXPrimitiveInstance* PrimitiveInstance = It.PrimitiveInstance;
						
			if (PrimitiveInstance->Primitive->GetMask() != 0)
			{

				const LXMatrix& MatrixWCS = It.MatrixWorld;
				const LXBBox& BBoxWorld = It.BBoxWorld;

				// Create and add
				LXRenderCluster* RenderCluster = CreateRenderCluster(ActorMesh, PrimitiveInstance, MatrixWCS, BBoxWorld, PrimitiveInstance->Primitive.get(), PrimitiveInstance->Primitive->GetMaterial());

				if (!RenderCluster)
					continue;
				
				// Cluster specialization
				if (Actor->GetCID() & LX_NODETYPE_LIGHT)
				{
					RenderCluster->Flags = ERenderClusterType::Light;
					RenderCluster->SetLightParameters(ActorMesh);
				}
				else if (Actor->GetCID() & LX_NODETYPE_CS)
				{
					RenderCluster->Flags = ERenderClusterType::Auxiliary;
				}
				else
				{	
					// Surface
					RenderCluster->CastShadow = ActorMesh->GetCastShadows();
				}

				// Create and add primitive bounds
				if (0)
				{
					LXMatrix MatrixScale, MatrixTranslation;
					// Max/1.f to avoid a 0 scale value ( possible with the "flat" geometries )
					MatrixScale.SetScale(max(BBoxWorld.GetSizeX(), 1.f), max(BBoxWorld.GetSizeY(), 1.f), max(BBoxWorld.GetSizeZ(), 1.f));
					MatrixTranslation.SetTranslation(BBoxWorld.GetCenter());
					const shared_ptr<LXPrimitive>& Primitive = GetPrimitiveFactory()->GetWireframeCube();
					CreateRenderCluster(ActorMesh, PrimitiveInstance, MatrixTranslation * MatrixScale, BBoxWorld, Primitive.get(), Primitive->GetMaterial());
					
					// For debug purpose. Creates a WireFrameCube primitive matching the BBoxWord. So no transformation is used.
					// Useful to verify the real BBoxWorld data
					/*
					LXPrimitive* Primitive = GetPrimitiveFactory()->CreateWireframeCube(BBoxWorld.GetMin().x, BBoxWorld.GetMin().y, BBoxWorld.GetMin().z,
					BBoxWorld.GetMax().x, BBoxWorld.GetMax().y, BBoxWorld.GetMax().z);
					LXMatrix MatrixIdentiy;
					CreateRenderCluster(ActorMesh, MatrixIdentiy, BBoxWorld, Primitive, Primitive->GetMaterial());
					*/
				}
			}
		}


		// Create and add actor bounds
		if (0)
		{
			const LXBBox& BBoxWorld = ActorMesh->GetBBoxWorld();

			LXMatrix MatrixScale, MatrixTranslation;
			// Max/1.f to avoid a 0 scale value ( possible with the "flat" geometries )
			MatrixScale.SetScale(max(BBoxWorld.GetSizeX(), 1.f), max(BBoxWorld.GetSizeY(), 1.f), max(BBoxWorld.GetSizeZ(), 1.f));
			MatrixTranslation.SetTranslation(BBoxWorld.GetCenter());
			LXPrimitive* Primitive = GetPrimitiveFactory()->GetWireframeCube().get();
			CreateRenderCluster(ActorMesh, nullptr, MatrixTranslation * MatrixScale, BBoxWorld, Primitive, Primitive->GetMaterial());

			// For debug purpose. Creates a WireFrameCube primitive matching the BBoxWord. So no transformation is used.
			// Useful to verify the real BBoxWorld data
			/*
			LXPrimitive* Primitive = GetPrimitiveFactory()->CreateWireframeCube(BBoxWorld.GetMin().x, BBoxWorld.GetMin().y, BBoxWorld.GetMin().z,
			BBoxWorld.GetMax().x, BBoxWorld.GetMax().y, BBoxWorld.GetMax().z);
			LXMatrix MatrixIdentiy;
			CreateRenderCluster(ActorMesh, MatrixIdentiy, BBoxWorld, Primitive, Primitive->GetMaterial());
			*/
		}
	}
}

// void LXRenderClusterManager::MoveActor(LXActor* Actor)
// {
// 	auto It = ActorRenderCluster.find(Actor);
// 	if (It != ActorRenderCluster.end())
// 	{
// 		for (LXRenderCluster* RenderCuster : It->second)
// 		{
// 			RenderCuster->UpdateMatrix();
// 		}
// 	}
// 	else
// 	{
// 		// Actor is not in renderer, Maybe not visible
// 	}
// }

void LXRenderClusterManager::UpdateActor(LXActor* Actor)
{
	if (Actor->IsRenderStateValid())
	{
		CHK(0);
		return;
	}

	// Remove previous RenderStates
	RemoveActor(Actor);

	// Try to add
	// Actor without parent is in "recycle bin" state
	if (Actor->IsVisible() && Actor->GetParent())
		AddActor(Actor);
}

void LXRenderClusterManager::RemoveActor(LXActor* Actor)
{
	list<LXRenderCluster*> ListRenderClusterToRemove;
	for (LXRenderCluster* RenderCluster : ListRenderClusters)
	{
		if (RenderCluster->Actor == Actor)
		{
			ListRenderClusterToRemove.push_back(RenderCluster);
		}
	}

	// This happens as we always trey to remove the actor before to add them.
// 	if (ListRenderClusterToRemove.size() == 0)
// 	{
// 		LogW(LXRenderClusterManager, L"RemoveActor: Actor %s has no cluster(s)", Actor->GetName().GetBuffer());
// 	}

	for (LXRenderCluster* RenderCluster : ListRenderClusterToRemove)
	{
		RemoveRenderCluster(RenderCluster);
		delete RenderCluster;
	}

	ActorRenderCluster.erase(Actor);


}

void LXRenderClusterManager::RemoveRenderCluster(LXRenderCluster* RenderCluster)
{
	CHK(RenderCluster);
	
	// Main list
	ListRenderClusters.remove(RenderCluster);
	
	// Helper maps
	PrimitiveInstanceRenderClusters.erase(RenderCluster->PrimitiveInstance);
}

void LXRenderClusterManager::UpdateMatrix(const LXRendererUpdateMatrix& RendererUpdateMatrix)
{
	LXPrimitiveInstance* PrimitiveInstance = RendererUpdateMatrix.PrimitiveInstance;
	CHK(PrimitiveInstance);

	auto It = PrimitiveInstanceRenderClusters.find(PrimitiveInstance);
	
	if (It != PrimitiveInstanceRenderClusters.end())
	{
		LXRenderCluster* RenderCluster = It->second;
		RenderCluster->SetMatrix(RendererUpdateMatrix.Matrix);
		RenderCluster->SetBBoxWorld(RendererUpdateMatrix.BBox);
	}
}


shared_ptr<LXPrimitiveD3D11>& LXRenderClusterManager::GetPrimitiveD3D11(LXPrimitive* Primitive, const ArrayVec3f* ArrayInstancePosition/* = nullptr*/)
{
	auto It = MapPrimitiveD3D11.find(pair<LXPrimitive*, uint>(Primitive, ArrayInstancePosition ? (uint)ArrayInstancePosition->size() : 0));

	if (It != MapPrimitiveD3D11.end())
	{
		return It->second;
	}
	else
	{
		const auto Key = pair<LXPrimitive*, uint>(Primitive, ArrayInstancePosition ? (uint)ArrayInstancePosition->size() : 0);
		MapPrimitiveD3D11[Key] = make_shared<LXPrimitiveD3D11>();
		MapPrimitiveD3D11[Key]->Create(Primitive, ArrayInstancePosition);
		return MapPrimitiveD3D11[Key];
	}
}

bool LXRenderClusterManager::GetShadersD3D11(ERenderPass renderPass, const LXPrimitiveD3D11* primitiveD3D11, const LXMaterialD3D11* materialD3D11, LXShaderProgramD3D11* shaderProgram)
{
	LXRenderer* Renderer = GetCore().GetRenderer();
	if (Renderer->GetShaderManager()->GetShaders(renderPass, primitiveD3D11, materialD3D11, shaderProgram))
	{
		return true;
	}
	else
	{
		return false;
	}
}

LXRenderCluster* LXRenderClusterManager::CreateRenderCluster(LXActorMesh* Actor, LXPrimitiveInstance* PrimitiveInstance, const LXMatrix& MatrixWCS, const LXBBox& BBoxWorld, LXPrimitive* Primitive, LXMaterial* Material)
{
	LogD(LXRenderClusterManager, L"CreateRenderCluster %s", Actor->GetName().GetBuffer());

	// Create the RenderCluster
	LXRenderCluster* RenderCluster = new LXRenderCluster(this, Actor, MatrixWCS);
	RenderCluster->SetBBoxWorld(BBoxWorld);

	// PrimitiveInstance 
	RenderCluster->PrimitiveInstance = PrimitiveInstance;
	PrimitiveInstance->RenderCluster = RenderCluster;

	// Create or Retrieve the PrimitiiveD3D11 according the Primitive
	shared_ptr<LXPrimitiveD3D11>& PrimitiveD3D11 = GetPrimitiveD3D11(Primitive, (Actor->GetInsanceCount()) > 0 ? &Actor->GetArrayInstancePosition() : nullptr);
	RenderCluster->SetPrimitive(PrimitiveD3D11);

	if (Material == nullptr)
	{
		Material = GetCore().GetDefaultMaterial();
		CHK(Material);
	}

	RenderCluster->SetMaterial(Material);

	ListRenderClusters.push_back(RenderCluster);
	ActorRenderCluster[Actor].push_back(RenderCluster);
	
	// May happen with the BoudingVolume
	CHK(PrimitiveInstanceRenderClusters.find(PrimitiveInstance) == PrimitiveInstanceRenderClusters.end());
	PrimitiveInstanceRenderClusters[PrimitiveInstance] = RenderCluster;

	return RenderCluster;
}
