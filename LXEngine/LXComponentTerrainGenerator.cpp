//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "LXComponentTerrainGenerator.h"

// Seetron
#include "LXActor.h"
#include "LXActorRenderToTexture.h"
#include "LXAssetManager.h"
#include "LXEngine.h"
#include "LXComponentMetaPatch.h"
#include "LXMaterial.h"
#include "LXMaterialBase.h"
#include "LXMaterialInstance.h"
#include "LXMath.h"
#include "LXPrimitive.h"
#include "LXPrimitiveInstance.h"
#include "LXPrimitiveFactory.h"
#include "LXScene.h"
#include "LXTexture.h"

LX_PRAGMA_OPTIM_OFF

namespace TerrainGenerator
{
	// Material property exposed in Material
	const LXString kHeightScale = L"HeightScale";
	const bool supportLOD = true;
}

LXComponentTerrainGenerator::LXComponentTerrainGenerator(LXActor* actor):
	LXComponent(actor)
{

}

LXComponentTerrainGenerator::~LXComponentTerrainGenerator()
{
}

void LXComponentTerrainGenerator::Build(const LXTerrainInfo& terrainInfo)
{
	CHK(!_building);
	if (_building)
		return;

	_building = true;

	CreateMetaPatches(terrainInfo);
	CreateGenerationResources(terrainInfo);
	GenerateHeigtMap(terrainInfo);
}

void LXComponentTerrainGenerator::CreateMetaPatches(const LXTerrainInfo& terrainInfo)
{
	//
	// Primitives
	//

	// Create the primitive LODs according the QuadPerPatchSize.
	std::vector<std::shared_ptr<LXPrimitive>> primitiveLODs;
	int quadPerPatchSide = terrainInfo._quadPerPatchSide;
	float quadSize = terrainInfo._quadSize;
	while (quadPerPatchSide > 1)
	{
		primitiveLODs.push_back(GetPrimitiveFactory()->CreateTerrainPatch(quadPerPatchSide, (int)quadSize));
		quadPerPatchSide /= 2;
		quadSize *= 2.f;

		if (primitiveLODs.size() == LX_MAX_LODS) 
			break; // TODO: log
	}

	for (int i = 0; i < primitiveLODs.size(); i++)
		primitiveLODs[i]->SetPersistent(false);

	//
	// Create and Add the MetaPatches.
	//

	// Release the previous ones
	for (LXActor* metaPatch : _actorMetaPatches)
	{
		_actor->RemoveChild(metaPatch);
		delete(metaPatch);
	}
	_actorMetaPatches.clear();

	const float patchSize = terrainInfo._quadSize * terrainInfo._quadPerPatchSide;
	const float metaPatchSize = patchSize * terrainInfo._patchPerMetaPatchSide;

	const LXPropertyAssetPtr* property = dynamic_cast<LXPropertyAssetPtr*>(_actor->GetProperty(L"Material"));
	const LXMaterialBase* material = (LXMaterialBase*)property->GetValue().get();

	float heightScale = 0.f;
	material->GetFloatParameter(TerrainGenerator::kHeightScale, heightScale);
		
	for (int y = 0; y < terrainInfo._metaPatchPerTerrainSide; y++)
	{
		for (int x = 0; x < terrainInfo._metaPatchPerTerrainSide; x++)
		{
			// Set the local (in the ActorTerrain coordinates) MetaPatch position.
			vec2f metaPatchPosition = vec2f(x * metaPatchSize, y * metaPatchSize);

			// MetaPatch is simple child Actor
			LXActor* actorMetaPatch = new LXActor();
			actorMetaPatch->SetPersistent(false);
			LXString metaPatchName = L"MetaPatch_" + LXString::Number(x) + LXString::Number(y);
			actorMetaPatch->SetName(metaPatchName);
			actorMetaPatch->SetPosition(vec3f(x * metaPatchSize, y * metaPatchSize, 0/* != 0 not supported*/));

			// Add a MetaPatchComponent.
			LXComponentMetaPatch& componentMetaPatch = actorMetaPatch->CreateAndAddComponent<LXComponentMetaPatch>();
			componentMetaPatch.CreateMainResources(terrainInfo,  metaPatchPosition);
			
			componentMetaPatch.Compiled.AttachMemberLambda([this, &componentMetaPatch]()
				{
					componentMetaPatch.InvalidateBounds();
				});

			// Update the World Bounds with the Terrain displacement.
			componentMetaPatch.PrimitiveWorldBoundsComputed.AttachMemberLambda([this, heightScale](LXComponent* component)
				{
					LXComponentMetaPatch* componentMesh = static_cast<LXComponentMetaPatch*>(component);
					LXRenderData* renderData = componentMesh->GetRenderData();

					if (!renderData)
						return;

					for (LXWorldPrimitive* worldPrimitive : renderData->GetPrimitives())
					{
						const std::shared_ptr<LXMaterialBase>& material = worldPrimitive->PrimitiveInstance->Material;
						const LXPropertyAssetPtr* property = dynamic_cast<LXPropertyAssetPtr*>(material->GetProperty(L"HeightMap"));
						const LXTexture* texture = (LXTexture*)property->GetValue().get();

						CHK(worldPrimitive->BBoxWorld.IsValid());
						vec2f zMinMax = componentMesh->GetMinMaxHeight(worldPrimitive, texture, heightScale);
						vec3f min = worldPrimitive->BBoxWorld.GetMin();
						vec3f max = worldPrimitive->BBoxWorld.GetMax();

						// Not needed if the MetaPatch are always at z = 0
						min.z = zMinMax.x + worldPrimitive->MatrixWorld.GetOrigin().z;
						max.z = zMinMax.y + worldPrimitive->MatrixWorld.GetOrigin().z;;

						worldPrimitive->BBoxWorld.Reset();
						worldPrimitive->BBoxWorld.Add(min);
						worldPrimitive->BBoxWorld.Add(max);
					}
				});

			// Picking
			componentMetaPatch.UpdateWorldPositionForPicking.AttachMemberLambda([this](LXComponent* component, vec3f& v0, vec3f& v1, vec3f& v2)
				{
					LXComponentMetaPatch* componentMesh = static_cast<LXComponentMetaPatch*>(component);
					v0.z = componentMesh->GetHeightAt(v0.x, v0.y);
					v1.z = componentMesh->GetHeightAt(v1.x, v1.y);
					v2.z = componentMesh->GetHeightAt(v2.x, v2.y);
				});

			for (int l = 0; l < terrainInfo._patchPerMetaPatchSide; l++)
			{
				for (int k = 0; k < terrainInfo._patchPerMetaPatchSide; k++)
				{
					LXMatrix Matrix;
					Matrix.SetOrigin(k * patchSize, l * patchSize, 0.f);

					if (TerrainGenerator::supportLOD)
					{
						for (int i = 0; i < primitiveLODs.size(); i++)
						{
							componentMetaPatch.AddPrimitive(primitiveLODs[i], &Matrix, componentMetaPatch.Material, i);
						}
					}
					else
					{
						componentMetaPatch.AddPrimitive(primitiveLODs[0], &Matrix, componentMetaPatch.Material, 0);
					}
				}
			}

			if (TerrainGenerator::supportLOD)
			{
				// TODO: in the LODs loop
				componentMetaPatch.SetLODData(0, M2CM(100.f));
				componentMetaPatch.SetLODData(1, M2CM(200.f));
				componentMetaPatch.SetLODData(2, M2CM(400.f));
				componentMetaPatch.SetLODData(3, M2CM(800.f));
				componentMetaPatch.SetLODData(4, FLT_MAX);
			}

			_actor->AddChild(actorMetaPatch);
			_actorMetaPatches.push_back(actorMetaPatch);
		}
	}

	//Terrain.InvalidateBounds(true);?
	//Terrain.InvalidateRenderState(); ?
}

void LXComponentTerrainGenerator::CreateGenerationResources(const LXTerrainInfo& terrainInfo)
{
#if LX_CMP_USEGENRESOURCES
	for (LXActor* actorMetaPatch : _actorMetaPatches)
	{
		if (LXComponentMetaPatch* componentMetaPatch = actorMetaPatch->GetComponent<LXComponentMetaPatch>())
		{
			vec3f translation = actorMetaPatch->GetTransformation().GetTranslation();
			translation /= terrainInfo.GetMetaPatchSize();
			componentMetaPatch->CreateBuildResources(terrainInfo, vec2f(translation.x, translation.y));
		}
	}
#endif
}

void LXComponentTerrainGenerator::GenerateHeigtMap(const LXTerrainInfo& terrainInfo)
{
	for (LXActor* actorMetaPatch : _actorMetaPatches)
	{
		if (LXComponentMetaPatch* componentMetaPatch = actorMetaPatch->GetComponent<LXComponentMetaPatch>())
		{
#if LX_CMP_USEGENRESOURCES
			componentMetaPatch->GenerateHeigtMap(terrainInfo);
#else

#endif
		}
	}
}

void LXComponentTerrainGenerator::Built()
{
	_building = false;
	_actor->InvalidateBounds(true);
}

LX_PRAGMA_OPTIM_ON