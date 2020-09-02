//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "LXRenderData.h"

// Seetron
#include "LXActorMesh.h"
#include "LXComponentMesh.h"
#include "LXCore.h"
#include "LXMesh.h"
#include "LXPrimitive.h"
#include "LXPrimitiveInstance.h"
#include "LXRenderCluster.h"
#include "LXRenderer.h"

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

void LXWorldPrimitive::SetMaterial(const LXMaterialBase* material)
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
}

LXRenderData::LXRenderData(LXActor* actor, LXComponent* component):
	_actor(actor),
	_component(component),
	_actorType(component->GetCID()),
	_bboxWorld(component->GetBBoxWorld()),
	_castShadows(component->GetCastShadows()),
	_primitiveBBox(component->IsPrimitiveBBoxVisible()),
	_actorBBox(component->IsBBoxVisible())
{
	LX_COUNTSCOPEINC(LXRenderData);

	if (const LXComponentMesh* componentMesh = dynamic_cast<const LXComponentMesh*>(component))
	{
		_mesh = componentMesh->Get();
		VectorPrimitiveInstances primitives;
		componentMesh->Get()->GetAllPrimitives(primitives);
		for (const shared_ptr<LXPrimitiveInstance>& primitiveInstance : primitives)
		{
			_worldPrimitives.push_back(new LXWorldPrimitive(primitiveInstance));
		}
	}
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
	_bboxWorld = _component?_component->GetBBoxWorld():_actor->GetBBoxWorld();
	CHK(_bboxWorld.IsValid());
	
	const LXMatrix& actorWorldMatrix = _component ? _component->GetMatrixWCS():_actor->GetMatrixWCS();
	for (LXWorldPrimitive* worldPrimitive : _worldPrimitives)
	{
		const LXMatrix* matrix = worldPrimitive->PrimitiveInstance->MatrixRCS;
		LXMatrix matrixMeshWCS = matrix ? actorWorldMatrix * *matrix : actorWorldMatrix;
		LXBBox BBoxWorld = worldPrimitive->PrimitiveInstance->Primitive->GetBBoxLocal();
		matrixMeshWCS.LocalToParent(BBoxWorld);
		worldPrimitive->MatrixWorld = matrixMeshWCS;
		worldPrimitive->BBoxWorld = BBoxWorld;
	}

	_validPrimitiveWorldMatrices = true;
	_validPrimitiveWorldBounds = true;
	
	_actor->OnComputedPrimitivesWordBBoxes();

	if (_component)
		_component->PrimitiveWorldBoundsComputed.Invoke(_component);

	LXTask* task = new LXTaskCallBack([this]()
	{
		for (LXRenderCluster* renderCluster : RenderClusters)
		{
			if (renderCluster->Role == LXFlagsRenderClusterRole(ERenderClusterRole::Default))
			{
				renderCluster->SetMatrix(renderCluster->PrimitiveInstance->MatrixWorld);
				renderCluster->SetBBoxWorld(renderCluster->PrimitiveInstance->BBoxWorld);
			}
			else if (renderCluster->Role == LXFlagsRenderClusterRole(ERenderClusterRole::ActorBBox))
			{
				const LXBBox& BBox = _bboxWorld;
				LXMatrix matrixScale, matrixTranslation;
				matrixScale.SetScale(max(BBox.GetSizeX(), 1.f), max(BBox.GetSizeY(), 1.f), max(BBox.GetSizeZ(), 1.f));
				matrixTranslation.SetTranslation(BBox.GetCenter());
				renderCluster->SetMatrix(matrixTranslation * matrixScale);
				renderCluster->SetBBoxWorld(BBox);
			}
			else
			{
				CHK(0); // Unknown role;
			}
		}
	});
	
	GetRenderer()->EnqueueTask(task);
}

void LXRenderData::ComputePrimitiveWorldBounds()
{
	_bboxWorld = _component?_component->GetBBoxWorld():_actor->GetBBoxWorld();
	CHK(_bboxWorld.IsValid());

	for (LXWorldPrimitive* worldPrimitive : _worldPrimitives)
	{
		LXBBox BBoxWorld = worldPrimitive->PrimitiveInstance->Primitive->GetBBoxLocal();
		worldPrimitive->MatrixWorld.LocalToParent(BBoxWorld);
		worldPrimitive->BBoxWorld = BBoxWorld;
	}

	_validPrimitiveWorldBounds = true;
	_actor->OnComputedPrimitivesWordBBoxes();
	
	if (_component)
		_component->PrimitiveWorldBoundsComputed.Invoke(_component);

	LXTask* task = new LXTaskCallBack([this]()
	{
		for (LXRenderCluster* renderCluster : RenderClusters)
		{
			if (renderCluster->Role == LXFlagsRenderClusterRole(ERenderClusterRole::Default))
			{
				renderCluster->SetBBoxWorld(renderCluster->PrimitiveInstance->BBoxWorld);
			}
			else if (renderCluster->Role == LXFlagsRenderClusterRole(ERenderClusterRole::ActorBBox))
			{
				const LXBBox& BBox = _bboxWorld;
				renderCluster->SetBBoxWorld(BBox);
			}
			else
			{
				CHK(0); // Unknown role;
			}
		}
	});

	GetRenderer()->EnqueueTask(task);
}
