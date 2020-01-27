//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "StdAfx.h"
#include "LXActorMesh.h"
#include "LXAnchor.h"
#include "LXAssetMesh.h"
#include "LXLogger.h"
#include "LXMSXMLNode.h"
#include "LXMaterial.h"
#include "LXMesh.h"
#include "LXPrimitive.h"
#include "LXPrimitiveInstance.h"
#include "LXProject.h"
#include "LXRenderCluster.h"
#include "LXSettings.h"
#include "LXThread.h"
#include "LXMemory.h" // --- Must be the last included ---

LXWorldPrimitive::LXWorldPrimitive(LXPrimitiveInstance* InPrimitiveInstance):
	PrimitiveInstance(InPrimitiveInstance)
{
	PrimitiveInstance->Owners.push_back(this);
}

LXWorldPrimitive::LXWorldPrimitive(LXPrimitiveInstance* InPrimitiveInstance, const LXMatrix& Matrix, LXBBox& BBox) :
	PrimitiveInstance(InPrimitiveInstance),
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

LXActorMesh::LXActorMesh():
LXActor(GetCore().GetProject())
{
	_nCID |= LX_NODETYPE_MESH;
	SetName(L"ActorMesh");
	DefineProperties();
}

LXActorMesh::LXActorMesh(LXProject* pDocument):
LXActor(pDocument)
{
	_nCID |= LX_NODETYPE_MESH;
	SetName(L"Mesh");
	DefineProperties();
}

LXActorMesh::~LXActorMesh(void)
{
	for (LXWorldPrimitive* worldPrimitive : _worldPrimitives)
	{
		delete worldPrimitive;
	}
}

void LXActorMesh::SetInstancePosition(uint i, const vec3f& Position)
{
	_ArrayInstancePosition.push_back(Position);
}

void LXActorMesh::MarkForDelete()
{
	__super::MarkForDelete();
	InvalidateRenderState();
}

/*virtual*/
void LXActorMesh::DefineProperties( ) 
{
	// --------------------------------------------------------------------------------------------------------------
	LXProperty::SetCurrentGroup(L"ActorMesh");
	// --------------------------------------------------------------------------------------------------------------
	
	auto PropertyMeshLayer = DefinePropertyInt(L"Layer", LXPropertyID::MESH_LAYER, &_nLayer);
	PropertyMeshLayer->SetMinMax(0, 1);
	
	//Outlines
	DefinePropertyBool(L"Outlines", LXPropertyID::MESH_OUTLINES, &_showOutlines);
	
	//CastShadows
	DefinePropertyBool(L"CastShadows", GetAutomaticPropertyID(), &_bCastShadows);

	//Asset
	auto PropAssetMesh = DefineProperty(L"AssetMesh", (LXAsset**)&_AssetMesh);
	PropAssetMesh->SetLambdaOnChange([this](LXPropertyAssetPtr* Property)
	{
		UpdateAssetMeshCallbacks();
		UpdateMesh();
	});
}

void LXActorMesh::SetInstanceCount(uint Count)
{
	_InstanceCount = Count;
	_ArrayInstancePosition.clear();
	_ArrayInstancePosition.reserve(Count);
}

void LXActorMesh::UpdateMesh()
{
	if (_AssetMesh)
	{
		if (Mesh != _AssetMesh->GetMesh())
		{
			Mesh = _AssetMesh->GetMesh();
		}
	}
}

void LXActorMesh::UpdateAssetMeshCallbacks()
{
	if (_AssetMesh != nullptr)
	{
		_AssetMesh->RegisterCB(this, L"TransformationChanged", [this](LXSmartObject* SmartObject) 
		{
			InvalidateMatrixWCS();
			InvalidateRenderState();
		});

		_AssetMesh->RegisterCB(this, L"MeshesBoundingBoxInvalidated", [this](LXSmartObject* SmartObject)
		{
			InvalidateBounds(true);
		});

		_AssetMesh->RegisterCB(this, L"VisibiltyChanged", [this](LXSmartObject* SmartObject)
		{
			_bValidWorldPrimitiveMatrices = false;
			InvalidateRenderState();
		});
	}
}

void LXActorMesh::OnPropertyChanged(LXProperty* Property) 
{
	__super::OnPropertyChanged(Property);

	switch (Property->GetID())
	{
	case LXPropertyID::VISIBILITY: 
		InvalidateRenderState();
		break;
	case LXPropertyID::POSITION:
	case LXPropertyID::ROTATION:
	case LXPropertyID::SCALE:
		break;
	}
}

void LXActorMesh::SetMesh(shared_ptr<LXMesh>& mesh)
{
	CHK(Mesh == nullptr);
	Mesh = mesh;
	InvalidateBounds(true);
}

void LXActorMesh::SetAssetMesh(LXAssetMesh* AssetMesh)
{
	CHK(_AssetMesh == nullptr);
	_AssetMesh = AssetMesh;
	InvalidateBounds(true);
	UpdateAssetMeshCallbacks();
	UpdateMesh();
}

void LXActorMesh::ComputeBBoxLocal()
{
	if (_BBoxLocal.IsValid())
		return;

	// Reset the BBox
	_BBoxLocal.Reset();

	if (Mesh)
	{
		const LXBBox& BBoxMesh = Mesh->GetBounds();

		if (BBoxMesh.IsValid())
			_BBoxLocal.Add(BBoxMesh);
	}

	if (!_BBoxLocal.IsValid())
	{
		LogD(ActorMesh, L"Set the default BBox size for %s", GetName().GetBuffer());
		_BBoxLocal.Add(LX_VEC3F_XYZ_50);
		_BBoxLocal.Add(LX_VEC3F_NXYZ_50);
	}
}

void LXActorMesh::AddPrimitive(const shared_ptr<LXPrimitive>& Primitive, LXMatrix* InMatrix, LXMaterial* InMaterial)
{	
	Mesh->AddPrimitive(Primitive, InMatrix, InMaterial);
	InvalidateBounds(true);
	InvalidateRenderState();
	_bValidWorldPrimitiveMatrices = false;
}

void LXActorMesh::ReleaseAllPrimitives()
{
	Mesh->RemoveAllPrimitives();
	InvalidateWorldPrimitives();
	InvalidateBounds(true);
	InvalidateRenderState();
	_bValidWorldPrimitiveMatrices = false;
}

void LXActorMesh::InvalidateWorldPrimitives()
{
	_worldPrimitives.clear();
}

const TWorldPrimitives& LXActorMesh::GetAllPrimitives()
{
	if (_worldPrimitives.size() == 0)
		GatherPrimitives();

	if (!_bValidWorldPrimitiveMatrices)
	{
		CHK(::GetCurrentThreadId() != RenderThread);
		ComputePrimitiveWorldMatrices();
		_bValidWorldPrimitiveMatrices = true;
	}

	return _worldPrimitives;
}

void LXActorMesh::ComputeBBoxWorld()
{
	if (_BBoxWorld.IsValid())
		return;

	__super::ComputeBBoxWorld();
	 
	LXBBox box;
	for (const LXWorldPrimitive* worldPrimitive : _worldPrimitives)
	{
		CHK(worldPrimitive->BBoxWorld.IsValid());
		box.Add(worldPrimitive->BBoxWorld);
	}
	_BBoxWorld.Add(box);
}

void LXActorMesh::GatherPrimitives()
{
	CHK(_worldPrimitives.size() == 0);

	if (Mesh)
	{
		vector<LXPrimitiveInstance*> primitives;
		Mesh->GetAllPrimitives(primitives);

		for (LXPrimitiveInstance* primitiveInstance : primitives)
		{
			_worldPrimitives.push_back(new LXWorldPrimitive(primitiveInstance));
		}
	}
}

void LXActorMesh::ComputePrimitiveWorldMatrices()
{
	for (LXWorldPrimitive* worldPrimitive : _worldPrimitives)
	{
		const LXMatrix* matrix = worldPrimitive->PrimitiveInstance->MatrixRCS;
		LXMatrix matrixMeshWCS = matrix ? GetMatrixWCS() * *matrix : GetMatrixWCS();
		LXBBox BBoxWorld = worldPrimitive->PrimitiveInstance->Primitive->GetBBoxLocal();
		matrixMeshWCS.LocalToParent(BBoxWorld);
		worldPrimitive->MatrixWorld = matrixMeshWCS;
		worldPrimitive->BBoxWorld = BBoxWorld;
	}
}

void LXActorMesh::OnInvalidateMatrixWCS()
{
	_bValidWorldPrimitiveMatrices = false;
}
