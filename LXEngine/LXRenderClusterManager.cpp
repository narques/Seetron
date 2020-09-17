//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "LXActorType.h"
#include "LXCore.h"
#include "LXDeviceResourceManager.h"
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
	}

void LXRenderClusterManager::Tick()
{
}

void LXRenderClusterManager::AddActor(LXRenderData* renderData, LXFlagsRenderClusterRole renderClusterRole)
{
	static const LXMatrix matrixIdentity;

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
		const TWorldPrimitives& WorldPrimitives0 = renderData->GetPrimitives(0);

		for (int i=0; i<WorldPrimitives0.size();i++)
		{
			LXWorldPrimitive* worldPrimitive = WorldPrimitives0[i];
			LXPrimitiveInstance* PrimitiveInstance = WorldPrimitives0[i]->PrimitiveInstance.get();
						
			if (PrimitiveInstance->Primitive->GetMask() != 0)
			{
				const LXMatrix& MatrixWCS = worldPrimitive->MatrixWorld;
				const LXMatrix& matrix = PrimitiveInstance->MatrixRCS?*PrimitiveInstance->MatrixRCS: matrixIdentity;
				const LXBBox& BBoxWorld = worldPrimitive->BBoxWorld;

				const LXMaterialBase* material = PrimitiveInstance->Material ? PrimitiveInstance->Material.get() : PrimitiveInstance->Primitive->GetMaterial().get();
				
				// Creates the default RenderCluster 
				if (renderClusterRole & ERenderClusterRole::Default)
				{
					vector<LXWorldPrimitive*> worldPrimitiveLODs;
					vector<LXPrimitive*> primitiveLODs;

					worldPrimitiveLODs.push_back(worldPrimitive);
					primitiveLODs.push_back(PrimitiveInstance->Primitive.get());

					for (int LODIndex = 1; LODIndex < LX_MAX_LODS; LODIndex++)
					{
						const TWorldPrimitives& worldPrimitives = renderData->GetPrimitives(LODIndex);
						if (worldPrimitives.size())
						{
							worldPrimitiveLODs.push_back(worldPrimitives[i]);
							primitiveLODs.push_back(worldPrimitives[i]->PrimitiveInstance->Primitive.get());
						}
					}
					
					LXRenderCluster* renderCluster = CreateRenderCluster(renderData, worldPrimitiveLODs, MatrixWCS, matrix, BBoxWorld, primitiveLODs, material);
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
			LXPrimitive* primitive = GetPrimitiveFactory()->GetWireframeCube().get();
			vector<LXPrimitive*> primitiveLODs;
			primitiveLODs.push_back(primitive);
			vector<LXWorldPrimitive*> empty;
			LXRenderCluster* renderClusterBBox = CreateRenderCluster(renderData, empty, MatrixTranslation * MatrixScale, matrixIdentity, BBoxWorld, primitiveLODs, primitive->GetMaterial().get());
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

	//
	// Remove from main list
	//

	for (LXRenderCluster* renderCuster : renderClustersToRemove)
	{
		ListRenderClusters.remove(renderCuster);
		delete renderCuster;
	}
}

LXRenderCluster* LXRenderClusterManager::CreateRenderCluster(LXRenderData* renderData, const vector<LXWorldPrimitive*>& worldPrimitiveLODs, const LXMatrix& MatrixWCS, const LXMatrix& matrix, const LXBBox& BBoxWorld, const vector<LXPrimitive*>& primitiveLODs, const LXMaterialBase* Material)
{
	LXRenderer* renderer = GetRenderer();

	// Create the RenderCluster
	LXRenderCluster* RenderCluster = new LXRenderCluster(this, renderData, MatrixWCS, matrix);
	RenderCluster->SetBBoxWorld(BBoxWorld);
	RenderCluster->LODCount = (int)worldPrimitiveLODs.size();

	for (int i = 0; i < worldPrimitiveLODs.size(); i++)
	{
		// Create or Retrieve the PrimitiiveD3D11 according the Primitive
		const shared_ptr<LXPrimitiveD3D11>& PrimitiveD3D11 = renderer->GetDeviceResourceManager()->GetPrimitive(primitiveLODs[i]);
		RenderCluster->SetPrimitive(PrimitiveD3D11, i);

		// PrimitiveInstance 
		RenderCluster->PrimitiveInstance[i] = worldPrimitiveLODs[i];
	}
	
	if (Material == nullptr)
	{
		Material = GetCore().GetDefaultMaterial();
		CHK(Material);
	}

	RenderCluster->SetMaterial(Material);

	ListRenderClusters.push_back(RenderCluster);
	ActorRenderCluster[renderData].push_back(RenderCluster);
	
	return RenderCluster;
}
