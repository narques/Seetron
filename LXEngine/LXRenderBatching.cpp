//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "LXRenderBatching.h"

// Seetron
#include "LXPrimitiveD3D11.h"
#include "LXRenderCluster.h"
#include "LXRenderData.h"
#include "LXPrimitiveInstance.h"
#include "LXStatistic.h"

LXRenderBatching::LXRenderBatching()
{

}

LXRenderBatching::~LXRenderBatching()
{
	_batchedRenderCusterOpaques.clear();
}

void LXRenderBatching::Clear()
{
	for (Map::iterator it = _batchedRenderCusterOpaques.begin(); it != _batchedRenderCusterOpaques.end(); it++)
	{
		it->second->Instances.clear();
		it->second->InstancedPrimitive = nullptr;
	}

	_batchedRenderCusterOpaques.clear();
}

void LXRenderBatching::Do(std::list<LXRenderCluster*>& renderClusters)
{
	LX_PERFOSCOPE(Batching);

	Clear();

	
	for (LXRenderCluster* renderCluster : renderClusters)
	{
		// Clear previous Instances
		//renderCluster->DisableInstanced();
		const LXMaterialBase* materialBase = renderCluster->Material;
		const LXPrimitiveD3D11* primitive = renderCluster->Primitive[renderCluster->CurrentLODIndex].get();
		Key key = Key(primitive, materialBase);

		const Map::iterator& itBatched = _batchedRenderCusterOpaques.find(key);
				
		if (itBatched != _batchedRenderCusterOpaques.end())
		{
			LXRenderCluster* batchedRenderCluster = itBatched->second;

			// Prepare the BatchedRenderCluster
			if (batchedRenderCluster->Instances.size() == 0)
			{
				// Add itself as instance
				batchedRenderCluster->AddInstance(LX_VEC3F_NULL);
			}

			// Add a new instance & remove RenderCluster from the list
			// Compute a instance position in the BatchedRenderCluster world.
			vec3f worldInstancePosition = LX_VEC3F_NULL;
			renderCluster->MatrixWCS.LocalToParentPoint(worldInstancePosition);
			batchedRenderCluster->MatrixWCS.ParentToLocalPoint(worldInstancePosition);
			batchedRenderCluster->AddInstance(worldInstancePosition);
		}
		else
		{
			// The RenderCluster becomes a potential BatchedRenderCluster
			_batchedRenderCusterOpaques[key] = renderCluster;
		}
	}

	renderClusters.clear();

	for (Map::iterator it = _batchedRenderCusterOpaques.begin(); it != _batchedRenderCusterOpaques.end(); it++)
	{
		LXRenderCluster* batchedRenderCluster = it->second;
		if (batchedRenderCluster->Instances.size() > 0)
		{
			// TODO: store the LXPrimitiveD3D11 in the PrmitiveManager
			std::shared_ptr<LXPrimitiveD3D11> primitive = std::make_shared<LXPrimitiveD3D11>();
			primitive->Create(batchedRenderCluster->PrimitiveInstance[batchedRenderCluster->CurrentLODIndex]->PrimitiveInstance->Primitive.get(), &batchedRenderCluster->Instances);
			batchedRenderCluster->InstancedPrimitive = primitive;
		}
		
		// Fill the given RenderCluster list
		renderClusters.push_back(batchedRenderCluster);
	}
}
