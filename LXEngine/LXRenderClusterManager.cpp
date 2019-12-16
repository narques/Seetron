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
#include "LXMaterial.h"
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

void LXRenderClusterManager::AddActor(LXActor* Actor, LXFlagsRenderClusterRole renderClusterRole)
{
	auto it = ActorRenderCluster.find(Actor);
	if (it != ActorRenderCluster.end())
	{
		for (const LXRenderCluster* renderCluster : it->second)
		{
			CHK(!(renderCluster->Role & renderClusterRole));
		}
	}
	
	if (Actor->GetCID() & LX_NODETYPE_ANCHOR)
		return;

	if (Actor->GetCID() & LX_NODETYPE_CAMERA)
		return;

	if (LXActorMesh* ActorMesh = LXCast<LXActorMesh>(Actor))
	{
		const TWorldPrimitives& WorldPrimitives = ActorMesh->GetAllPrimitives();

		for(auto &It:WorldPrimitives)
		{
			LXPrimitiveInstance* PrimitiveInstance = It->PrimitiveInstance;
						
			if (PrimitiveInstance->Primitive->GetMask() != 0)
			{

				const LXMatrix& MatrixWCS = It->MatrixWorld;
				const LXBBox& BBoxWorld = It->BBoxWorld;
				LXWorldPrimitive* worldPrimitive = const_cast<LXWorldPrimitive*>(It);

				// Creates the default RenderCluster 
				if (renderClusterRole & ERenderClusterRole::Default)
				{
					LXRenderCluster* renderCluster = CreateRenderCluster(ActorMesh, worldPrimitive, MatrixWCS, BBoxWorld, PrimitiveInstance->Primitive.get(), PrimitiveInstance->Primitive->GetMaterial());
					renderCluster->Role = ERenderClusterRole::Default;
					worldPrimitive->RenderCluster = renderCluster;

					// Cluster specialization
					if (Actor->GetCID() & LX_NODETYPE_LIGHT)
					{
						renderCluster->Flags = ERenderClusterType::Light;
						renderCluster->SetLightParameters(ActorMesh);
					}
					else if (Actor->GetCID() & LX_NODETYPE_CS)
					{
						renderCluster->Flags = ERenderClusterType::Auxiliary;
					}
					else
					{
						// Surface
						renderCluster->CastShadow = ActorMesh->GetCastShadows();
					}
				}

				// Creates the primitive bounds RenderCluster
				if (renderClusterRole & ERenderClusterRole::PrimitiveBBox && 
					Actor->IsPrimitiveBBoxVisible() &&
					!(Actor->GetCID() & LX_NODETYPE_CS))
				{
					LXMatrix MatrixScale, MatrixTranslation;
					// Max/1.f to avoid a 0 scale value ( possible with the "flat" geometries )
					MatrixScale.SetScale(max(BBoxWorld.GetSizeX(), 1.f), max(BBoxWorld.GetSizeY(), 1.f), max(BBoxWorld.GetSizeZ(), 1.f));
					MatrixTranslation.SetTranslation(BBoxWorld.GetCenter());
					const shared_ptr<LXPrimitive>& Primitive = GetPrimitiveFactory()->GetWireframeCube();
					LXRenderCluster* renderClusterBBox = CreateRenderCluster(ActorMesh, worldPrimitive, /*MatrixWCS*/MatrixTranslation * MatrixScale, BBoxWorld, Primitive.get(), Primitive->GetMaterial());
					renderClusterBBox->Flags = ERenderClusterType::Auxiliary;
					renderClusterBBox->Role = ERenderClusterRole::PrimitiveBBox;
					
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


		// Creates the actor bounds RenderCluster
		if (renderClusterRole & ERenderClusterRole::ActorBBox && 
			Actor->IsBBoxVisible())
		{
			const LXBBox& BBoxWorld = ActorMesh->GetBBoxWorld();

			LXMatrix MatrixScale, MatrixTranslation;
			// Max/1.f to avoid a 0 scale value ( possible with the "flat" geometries )
			MatrixScale.SetScale(max(BBoxWorld.GetSizeX(), 1.f), max(BBoxWorld.GetSizeY(), 1.f), max(BBoxWorld.GetSizeZ(), 1.f));
			MatrixTranslation.SetTranslation(BBoxWorld.GetCenter());
			LXPrimitive* Primitive = GetPrimitiveFactory()->GetWireframeCube().get();
			LXRenderCluster* renderClusterBBox = CreateRenderCluster(ActorMesh, nullptr, MatrixTranslation * MatrixScale, BBoxWorld, Primitive, Primitive->GetMaterial());
			renderClusterBBox->Flags = ERenderClusterType::Auxiliary;
			renderClusterBBox->Role = ERenderClusterRole::ActorBBox;

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

void LXRenderClusterManager::UpdateActor(LXActor* Actor, LXFlagsRenderClusterRole renderClusterRole)
{
	if (Actor->IsRenderStateValid())
	{
		CHK(0);
		return;
	}

	// Remove previous RenderStates
	RemoveActor(Actor, renderClusterRole);

	// Try to add
	// Actor without parent is in "recycle bin" state
	if (Actor->IsVisible() && Actor->GetParent())
		AddActor(Actor, renderClusterRole);
}

void LXRenderClusterManager::RemoveActor(LXActor* Actor, LXFlagsRenderClusterRole renderClusterRole)
{

	list<LXRenderCluster*> renderClustersToRemove;

	//
	// Remove from helper maps
	//

	ActorRenderCluster[Actor].remove_if([renderClusterRole, &renderClustersToRemove](LXRenderCluster* renderCluster)
	{
		if (renderCluster->Role & renderClusterRole)
		{
			renderClustersToRemove.push_back(renderCluster);
			return true;
		}
		else
		{
			return false;
		}
	});

	if (ActorRenderCluster[Actor].size() == 0)
		ActorRenderCluster.erase(Actor);


	for (LXRenderCluster* renderCuster : renderClustersToRemove)
	{
		
		PrimitiveInstanceRenderClusters.erase(renderCuster->PrimitiveInstance);
	}
	
	//
	// Remove from main list
	//

	for (LXRenderCluster* renderCuster : renderClustersToRemove)
	{
		ListRenderClusters.remove(renderCuster);
		delete renderCuster;
	}
}


void LXRenderClusterManager::UpdateMatrix(const LXRendererUpdateMatrix& RendererUpdateMatrix)
{
	LXWorldPrimitive* worldPrimitive = RendererUpdateMatrix.WorldPrimitive;
	CHK(worldPrimitive);

	auto range = PrimitiveInstanceRenderClusters.equal_range(worldPrimitive);
	
	for ( auto it = range.first; it != range.second; it++)
	{
		LXRenderCluster* RenderCluster = it->second;
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

LXRenderCluster* LXRenderClusterManager::CreateRenderCluster(LXActorMesh* Actor, LXWorldPrimitive* worldPrimitive, const LXMatrix& MatrixWCS, const LXBBox& BBoxWorld, LXPrimitive* Primitive, LXMaterial* Material)
{
	//LogD(LXRenderClusterManager, L"CreateRenderCluster %s (%s)", Actor->GetName().GetBuffer(), Material?Material->GetName().GetBuffer():L"No material");

	// Create the RenderCluster
	LXRenderCluster* RenderCluster = new LXRenderCluster(this, Actor, MatrixWCS);
	RenderCluster->SetBBoxWorld(BBoxWorld);

	// PrimitiveInstance 
	RenderCluster->PrimitiveInstance = worldPrimitive;

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
	
	PrimitiveInstanceRenderClusters.insert(pair<LXWorldPrimitive*, LXRenderCluster*>(worldPrimitive, RenderCluster));

	return RenderCluster;
}
