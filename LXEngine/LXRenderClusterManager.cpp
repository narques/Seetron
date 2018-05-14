//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "LXActor.h"
#include "LXActorMesh.h"
#include "LXController.h"
#include "LXCore.h"
#include "LXLogger.h"
#include "LXMaterial.h"
#include "LXMaterialD3D11.h"
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
	MaterialRenderClusters.clear();
	MapPrimitiveD3D11.clear();
	MapMaterialD3D11.clear();
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

	auto It2 = MapMaterialD3D11.begin();
	while (It2 != MapMaterialD3D11.end())
	{
		// Delete the MaterialD3D11 when referenced by the map only
		if (It2->second.use_count() == 1)
		{
			auto toErase = It2;
			++It2;
			MapMaterialD3D11.erase(toErase);
		}
		else
		{
			++It2;
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
		const TWorldPrimitives& WorldPrimitives = ActorMesh->GetAllPrimitives();

		for(auto &It:WorldPrimitives)
		{
			LXPrimitiveInstance* PrimitiveInstance = It.PrimitiveInstance;
						
			if (PrimitiveInstance->Primitive->GetMask() != 0)
			{

				const LXMatrix& MatrixWCS = It.MatrixWorld;
				const LXBBox& BBoxWorld = It.BBoxWorld;

				// Create and add
				LXRenderCluster* RenderCluster = CreateRenderCluster(ActorMesh, PrimitiveInstance, MatrixWCS, BBoxWorld, PrimitiveInstance->Primitive.get(), PrimitiveInstance->Primitive->GetMaterial());
				
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
	/*
	map<LXActor*, list<LXRenderCluster*>> ActorRenderCluster;
	
	
	*/

	LXMaterial* Material = const_cast<LXMaterial*>(RenderCluster->Material->GetMaterial());
	CHK(Material);
	MaterialRenderClusters[Material].remove(RenderCluster);
	
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

void LXRenderClusterManager::UpdateMaterial(const LXMaterial* Material)
{
	if (LXMaterialD3D11* MaterialD3D11 = GetMaterial(Material))
	{
		MaterialD3D11->Update(Material);
	}
}

shared_ptr<LXMaterialD3D11>& LXRenderClusterManager::GetMaterialD3D11(const LXMaterial* Material)
{
	if (!Material)
		Material = GetCore().GetDefaultMaterial();
	
	auto It = MapMaterialD3D11.find(Material);

	if (It != MapMaterialD3D11.end())
	{
		return It->second;
	}
	else
	{
		MapMaterialD3D11[Material] = make_shared<LXMaterialD3D11>(Material);;
		return MapMaterialD3D11[Material];
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

shared_ptr<LXMaterialD3D11>& LXRenderClusterManager::GetMaterialAndShadersD3D11(ERenderPass RenderPass, const LXMaterial* Material, const LXPrimitiveD3D11* PrimitiveD3D11, LXShaderProgramD3D11* OutShaderProgram)
{
	LXRenderer* Renderer = GetCore().GetRenderer();
	shared_ptr<LXMaterialD3D11>& MaterialD3D11 = GetMaterialD3D11(Material);

	if (Renderer->GetShaderManager()->GetShaders(RenderPass, PrimitiveD3D11, MaterialD3D11.get(), OutShaderProgram))
	{
		return MaterialD3D11;
	}
	else
	{
		// Try with the default material
		MaterialD3D11 = GetMaterialD3D11(nullptr);

		if (Renderer->GetShaderManager()->GetShaders(RenderPass, PrimitiveD3D11, MaterialD3D11.get(), OutShaderProgram))
		{
			return MaterialD3D11;
		}
		else
		{
			static shared_ptr<LXMaterialD3D11> NullMaterialD3D11;
			return NullMaterialD3D11;
		}
	}
}

// void LXRenderClusterManager::RebuildMaterial(const LXMaterial* Material)
// {
// 	auto It = MaterialRenderClusters.find(Material);
// 	if (It != MaterialRenderClusters.end())
// 	{
// 		for (LXRenderCluster* RenderCluster : (*It).second)
// 		{
// 			LXPrimitiveD3D11* PrimitiveD3D11 = RenderCluster->Primitive.get();
// 			LXShaderProgramD3D11 ShaderProgram;
// 
// 			shared_ptr<LXMaterialD3D11>& NewMaterialD3D11 = GetMaterialAndShadersD3D11(ERenderPass::GBuffer, Material, PrimitiveD3D11, &ShaderProgram);
// 			if (NewMaterialD3D11.get())
// 			{
// 				LXShaderProgramD3D11* RenderClusterShaderProgram = &RenderCluster->ShaderPrograms[(int)ERenderPass::GBuffer];
// 				RenderCluster->SetMaterial(NewMaterialD3D11);
// 				RenderClusterShaderProgram->VertexShader = ShaderProgram.VertexShader;
// 				RenderClusterShaderProgram->HullShader = ShaderProgram.HullShader;
// 				RenderClusterShaderProgram->DomainShader = ShaderProgram.DomainShader;
// 				RenderClusterShaderProgram->GeometryShader = ShaderProgram.GeometryShader;
// 				RenderClusterShaderProgram->PixelShader = ShaderProgram.PixelShader;
// 			}
// 			else
// 			{
// 				CHK(0);
// 			}
// 		}
// 	}
// }

LXMaterialD3D11* LXRenderClusterManager::GetMaterial(const LXMaterial* Material)
{
	auto It = MapMaterialD3D11.find(Material);
	if (It != MapMaterialD3D11.end())
	{
		return (*It).second.get();
	}
	else
	{
		return nullptr;
	}
}

LXRenderCluster* LXRenderClusterManager::CreateRenderCluster(LXActorMesh* Actor, LXPrimitiveInstance* PrimitiveInstance, const LXMatrix& MatrixWCS, const LXBBox& BBoxWorld, LXPrimitive* Primitive, LXMaterial* Material)
{
	//LogD(LXRenderClusterManager, L"CreateRenderCluster %s", Actor->GetName().GetBuffer());

	// Create the RenderCluster
	LXRenderCluster* RenderCluster = new LXRenderCluster(this, Actor, MatrixWCS);
	RenderCluster->SetBBoxWorld(BBoxWorld);

	// PrimitiveInstance 
	RenderCluster->PrimitiveInstance = PrimitiveInstance;

	// Create or Retrieve the PrimitiiveD3D11 according the Primitive
	shared_ptr<LXPrimitiveD3D11>& PrimitiveD3D11 = GetPrimitiveD3D11(Primitive, (Actor->GetInsanceCount()) > 0 ? &Actor->GetArrayInstancePosition() : nullptr);
	RenderCluster->SetPrimitive(PrimitiveD3D11);
		
	// Create or retrieve the Shaders and MaterialD3D11 according the Material
	for (auto i = 0; i < (int)ERenderPass::Last; i++)
	{
		ERenderPass RenderPass = (ERenderPass)i;

		LXShaderProgramD3D11 ShaderProgram;

		shared_ptr<LXMaterialD3D11>& MaterialD3D11 = GetMaterialAndShadersD3D11(RenderPass, Material, PrimitiveD3D11.get(), &ShaderProgram);
		if (MaterialD3D11.get())
		{
			RenderCluster->SetMaterial(MaterialD3D11);
			LXShaderProgramD3D11* RenderClusterShaderProgram = &RenderCluster->ShaderPrograms[(int)RenderPass];
			RenderClusterShaderProgram->VertexShader = ShaderProgram.VertexShader;
			RenderClusterShaderProgram->HullShader = ShaderProgram.HullShader;
			RenderClusterShaderProgram->DomainShader = ShaderProgram.DomainShader;
			RenderClusterShaderProgram->GeometryShader = ShaderProgram.GeometryShader;
			RenderClusterShaderProgram->PixelShader = ShaderProgram.PixelShader;
		}
		else
		{
			//CHK(0);
			LogE(RenderClusterManager, L"Unable to retrieve a material for cluster");
			delete RenderCluster;
			return nullptr;
			
		}
	}

	ListRenderClusters.push_back(RenderCluster);
	ActorRenderCluster[Actor].push_back(RenderCluster);
	MaterialRenderClusters[Material].push_back(RenderCluster);

	// May happen with the BoudingVolume
	CHK(PrimitiveInstanceRenderClusters.find(PrimitiveInstance) == PrimitiveInstanceRenderClusters.end());
	PrimitiveInstanceRenderClusters[PrimitiveInstance] = RenderCluster;

	return RenderCluster;
}
