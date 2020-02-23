//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "LXActorMesh.h"
#include "LXController.h"
#include "LXCore.h"
#include "LXMesh.h"
#include "LXPrimitive.h"
#include "LXPrimitiveInstance.h"
#include "LXRenderCluster.h"
#include "LXRenderData.h"
#include "LXMemory.h" // --- Must be the last included ---

LXWorldPrimitive::LXWorldPrimitive(const shared_ptr<LXPrimitiveInstance>& primitiveInstance):
	PrimitiveInstance(primitiveInstance)
{
	PrimitiveInstance->Owners.push_back(this);
}

LXWorldPrimitive::LXWorldPrimitive(const shared_ptr<LXPrimitiveInstance>& primitiveInstance, const LXMatrix& Matrix, const LXBBox& BBox) :
	PrimitiveInstance(primitiveInstance),
	MatrixWorld(Matrix),
	BBoxWorld(BBox)
{
	PrimitiveInstance->Owners.push_back(this);
}

LXWorldPrimitive::~LXWorldPrimitive()
{
	PrimitiveInstance->Owners.remove(this);
}

void LXWorldPrimitive::SetMaterial(LXMaterial* material)
{
	if (RenderCluster)
	{
		RenderCluster->SetMaterial(material);
	}
}

LXRenderData::LXRenderData(LXActor* actor):
	_actor(actor),
	_actorType(actor->GetCID()),
	_bboxWorld(actor->GetBBoxWorld()),
	_castShadows(actor->GetCastShadows()),
	_primitiveBBox(actor->IsPrimitiveBBoxVisible()),
	_actorBBox(actor->IsBBoxVisible())
{
	LX_COUNTSCOPEINC(LXRenderData);

	if (LXActorMesh* actorMesh = dynamic_cast<LXActorMesh*>(actor))
	{
		_mesh = actorMesh->GetMesh();
		VectorPrimitiveInstances primitives;
		_mesh->GetAllPrimitives(primitives);
		for (const shared_ptr<LXPrimitiveInstance>& primitiveInstance : primitives)
		{
			_worldPrimitives.push_back(new LXWorldPrimitive(primitiveInstance));
		}
	}

	ComputePrimitiveWorldMatrices();
}

LXRenderData::~LXRenderData()
{
	LX_COUNTSCOPEDEC(LXRenderData);

	for (LXWorldPrimitive* worldPrimitive : _worldPrimitives)
	{
		delete worldPrimitive;
	}
}

void LXRenderData::ComputePrimitiveWorldMatrices()
{
	const LXMatrix& actorWorldMatrix = _actor->GetMatrixWCS();
	for (LXWorldPrimitive* worldPrimitive : _worldPrimitives)
	{
		const LXMatrix* matrix = worldPrimitive->PrimitiveInstance->MatrixRCS;
		LXMatrix matrixMeshWCS = matrix ? actorWorldMatrix * *matrix : actorWorldMatrix;
		LXBBox BBoxWorld = worldPrimitive->PrimitiveInstance->Primitive->GetBBoxLocal();
		matrixMeshWCS.LocalToParent(BBoxWorld);
		worldPrimitive->MatrixWorld = matrixMeshWCS;
		worldPrimitive->BBoxWorld = BBoxWorld;
	}

	GetController()->ActorWorldMatrixChanged(_actor);
}

