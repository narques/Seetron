//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "LXActorType.h"
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
#include "LXRenderData.h"
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

void LXRenderClusterManager::AddActor(LXRenderData* renderData, LXFlagsRenderClusterRole renderClusterRole)
{
	auto it = ActorRenderCluster.find(renderData);
	if (it != ActorRenderCluster.end())
	{
		for (const LXRenderCluster* renderCluster : it->second)
		{
			CHK(!(renderCluster->Role & renderClusterRole));
		}
	}
	
	if (renderData->GetActorType() & LX_NODETYPE_ANCHOR)
		return;

	if (renderData->GetActorType() & LX_NODETYPE_CAMERA)
		return;

	{
		const TWorldPrimitives& WorldPrimitives = renderData->GetPrimitives();

		for(auto &It:WorldPrimitives)
		{
			LXPrimitiveInstance* PrimitiveInstance = It->PrimitiveInstance.get();
						
			if (PrimitiveInstance->Primitive->GetMask() != 0)
			{

				const LXMatrix& MatrixWCS = It->MatrixWorld;
				const LXBBox& BBoxWorld = It->BBoxWorld;
				LXWorldPrimitive* worldPrimitive = const_cast<LXWorldPrimitive*>(It);

				// Creates the default RenderCluster 
				if (renderClusterRole & ERenderClusterRole::Default)
				{
					LXRenderCluster* renderCluster = CreateRenderCluster(renderData, worldPrimitive, MatrixWCS, BBoxWorld, PrimitiveInstance->Primitive.get(), PrimitiveInstance->Primitive->GetMaterial().get());
					renderCluster->Role = ERenderClusterRole::Default;
					worldPrimitive->RenderCluster = renderCluster;
					
					// Cluster specialization
					if (renderData->GetActorType() & LX_NODETYPE_LIGHT)
					{
						renderCluster->Flags = ERenderClusterType::Light;
						renderCluster->SetLightParameters(const_cast<LXActor*>(renderData->GetActor()));
					}
					else if (renderData->GetActorType() & LX_NODETYPE_CS)
					{
						renderCluster->Flags = ERenderClusterType::Auxiliary;
					}
					else if (renderData->GetActorType() & LX_NODETYPE_RENDERTOTTEXTURE)
					{
						renderCluster->Flags = ERenderClusterType::RenderToTexture;
					}
					else
					{
						// Surface
						renderCluster->CastShadow = renderData->GetCastShadows();
					}
				}

				// Creates the primitive bounds RenderCluster
				if (renderClusterRole & ERenderClusterRole::PrimitiveBBox && 
					renderData->ShowPrimitiveBBox() &&
					!(renderData->GetActorType() & LX_NODETYPE_CS))
				{
#if 1
					LXMatrix MatrixScale, MatrixTranslation;
					// Max/1.f to avoid a 0 scale value ( possible with the "flat" geometries )
					MatrixScale.SetScale(max(BBoxWorld.GetSizeX(), 1.f), max(BBoxWorld.GetSizeY(), 1.f), max(BBoxWorld.GetSizeZ(), 1.f));
					MatrixTranslation.SetTranslation(BBoxWorld.GetCenter());
					const shared_ptr<LXPrimitive>& Primitive = GetPrimitiveFactory()->GetWireframeCube();
					LXRenderCluster* renderClusterBBox = CreateRenderCluster(renderData, worldPrimitive, MatrixTranslation * MatrixScale, BBoxWorld, Primitive.get(), Primitive->GetMaterial().get());
#else				
					// For debug purpose. Creates a WireFrameCube primitive matching the BBoxWord. So no transformation is used.
					// Useful to verify the real BBoxWorld data
					const shared_ptr <LXPrimitive> Primitive = GetPrimitiveFactory()->CreateWireframeCube(BBoxWorld.GetMin().x, BBoxWorld.GetMin().y, BBoxWorld.GetMin().z,
						BBoxWorld.GetMax().x, BBoxWorld.GetMax().y, BBoxWorld.GetMax().z);
					LXMatrix MatrixIdentiy;
					LXRenderCluster* renderClusterBBox = CreateRenderCluster(renderData, worldPrimitive, MatrixIdentiy, BBoxWorld, Primitive.get(), Primitive->GetMaterial().get());
#endif
					renderClusterBBox->Flags = ERenderClusterType::Auxiliary;
					renderClusterBBox->Role = ERenderClusterRole::PrimitiveBBox;
					
				}
			}
		}


		// Creates the actor bounds RenderCluster
		if (renderClusterRole & ERenderClusterRole::ActorBBox && 
			renderData->ShowActorBBox())
		{
			const LXBBox& BBoxWorld = renderData->GetBBoxWorld();

			LXMatrix MatrixScale, MatrixTranslation;
			// Max/1.f to avoid a 0 scale value ( possible with the "flat" geometries )
			MatrixScale.SetScale(max(BBoxWorld.GetSizeX(), 1.f), max(BBoxWorld.GetSizeY(), 1.f), max(BBoxWorld.GetSizeZ(), 1.f));
			MatrixTranslation.SetTranslation(BBoxWorld.GetCenter());
			LXPrimitive* Primitive = GetPrimitiveFactory()->GetWireframeCube().get();
			LXRenderCluster* renderClusterBBox = CreateRenderCluster(renderData, nullptr, MatrixTranslation * MatrixScale, BBoxWorld, Primitive, Primitive->GetMaterial().get());
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

void LXRenderClusterManager::UpdateActor(LXRenderData* renderData, LXFlagsRenderClusterRole renderClusterRole)
{
		AddActor(renderData, renderClusterRole);
}

void LXRenderClusterManager::RemoveActor(LXRenderData* renderData, LXFlagsRenderClusterRole renderClusterRole)
{

	list<LXRenderCluster*> renderClustersToRemove;

	//
	// Remove from helper maps
	//

	ActorRenderCluster[renderData].remove_if([renderClusterRole, &renderClustersToRemove](LXRenderCluster* renderCluster)
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

	if (ActorRenderCluster[renderData].size() == 0)
		ActorRenderCluster.erase(renderData);


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

		if (RenderCluster->Role == LXFlagsRenderClusterRole(ERenderClusterRole::Default))
		{
			RenderCluster->SetMatrix(RendererUpdateMatrix.Matrix);
			RenderCluster->SetBBoxWorld(RendererUpdateMatrix.BBox);
		}
		else if(RenderCluster->Role == LXFlagsRenderClusterRole(ERenderClusterRole::PrimitiveBBox))
		{
			LXMatrix matrixScale, matrixTranslation;
			matrixScale.SetScale(max(RendererUpdateMatrix.BBox.GetSizeX(), 1.f), max(RendererUpdateMatrix.BBox.GetSizeY(), 1.f), max(RendererUpdateMatrix.BBox.GetSizeZ(), 1.f));
			matrixTranslation.SetTranslation(RendererUpdateMatrix.BBox.GetCenter());
			RenderCluster->SetMatrix(matrixTranslation * matrixScale);
			RenderCluster->SetBBoxWorld(RendererUpdateMatrix.BBox);
		}

		// TODO: ActorBBox clusters are not in the PrimitiveInstanceRenderClusters.
		// We could use ActorRenderCluster (renderData as key) replacing the Actor in LXRendererUpdateMatrix.
		/*
		else if (RenderCluster->Role == LXFlagsRenderClusterRole(ERenderClusterRole::ActorBBox))
		{
			const LXBBox& BBox = RenderCluster->RenderData->GetBBoxWorld();
			LXMatrix matrixScale, matrixTranslation;
			matrixScale.SetScale(max(BBox.GetSizeX(), 1.f), max(BBox.GetSizeY(), 1.f), max(BBox.GetSizeZ(), 1.f));
			matrixTranslation.SetTranslation(BBox.GetCenter());
			RenderCluster->SetMatrix(matrixTranslation * matrixScale);
			RenderCluster->SetBBoxWorld(BBox);
		}
		*/
		else
		{
			CHK(0); // Unknown role;
		}
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

LXRenderCluster* LXRenderClusterManager::CreateRenderCluster(LXRenderData* renderData, LXWorldPrimitive* worldPrimitive, const LXMatrix& MatrixWCS, const LXBBox& BBoxWorld, LXPrimitive* Primitive, LXMaterial* Material)
{
	// Create the RenderCluster
	LXRenderCluster* RenderCluster = new LXRenderCluster(this, renderData, MatrixWCS);
	RenderCluster->SetBBoxWorld(BBoxWorld);

	// PrimitiveInstance 
	RenderCluster->PrimitiveInstance = worldPrimitive;

	// Create or Retrieve the PrimitiiveD3D11 according the Primitive
	shared_ptr<LXPrimitiveD3D11>& PrimitiveD3D11 = GetPrimitiveD3D11(Primitive, /*(actorMesh->GetInsanceCount()) > 0 ? &actorMesh->GetArrayInstancePosition() :*/ nullptr);
	RenderCluster->SetPrimitive(PrimitiveD3D11);

	if (Material == nullptr)
	{
		Material = GetCore().GetDefaultMaterial();
		CHK(Material);
	}

	RenderCluster->SetMaterial(Material);

	ListRenderClusters.push_back(RenderCluster);
	ActorRenderCluster[renderData].push_back(RenderCluster);
	
	PrimitiveInstanceRenderClusters.insert(pair<LXWorldPrimitive*, LXRenderCluster*>(worldPrimitive, RenderCluster));

	return RenderCluster;
}
