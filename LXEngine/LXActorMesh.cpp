//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
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
#include "LXPrimitiveInstance.h"
#include "LXProject.h"
#include "LXSettings.h"
#include "LXThread.h"
#include "LXMemory.h" // --- Must be the last included ---

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
			_bValidWorldPrimitives = false;
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

void LXActorMesh::SetMesh(LXMesh* InMesh)
{
	CHK(Mesh == nullptr);
	Mesh = InMesh;
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

void LXActorMesh::ComputeBBox()
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

		_BBoxLocal.ExtendZ(_ExtendZ);
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
}

void LXActorMesh::ReleaseAllPrimitives()
{
	Mesh->RemoveAllPrimitives();
	InvalidateWorldPrimitives();
	InvalidateBounds(true);
	InvalidateRenderState();
}

void LXActorMesh::InvalidateWorldPrimitives()
{
	_bValidWorldPrimitives = false;
}

const TWorldPrimitives& LXActorMesh::GetAllPrimitives(bool bIgnoreValidity)
{
	if (_bValidWorldPrimitives || bIgnoreValidity)
	{
		return _WorldPrimitives;
	}
	else
	{
		CHK(!IsRenderThread())
		_WorldPrimitives.clear();
		GetAllPrimitives(Mesh, _WorldPrimitives, GetMatrixWCS());
		_bValidWorldPrimitives = true;
		return _WorldPrimitives;
	}
}

const LXWorldPrimitive* LXActorMesh::GetWorldPrimitive(const LXPrimitiveInstance* PrimitiveInstance)
{
	const TWorldPrimitives& WorldPrimitives = GetAllPrimitives();

	for (const LXWorldPrimitive& WorldPrimitive : WorldPrimitives)
	{
		if (WorldPrimitive.PrimitiveInstance == PrimitiveInstance)
		{
			return &WorldPrimitive;
		}
	}

	// The PrimitiveInstance does not exist in this object.
	CHK(0);
	return nullptr;
}

void LXActorMesh::GetAllPrimitives(LXMesh* InMesh, TWorldPrimitives& OutWorldPrimitives, const LXMatrix& MatrixWCSParent)
{
	if (!InMesh || !InMesh->Visible())
	{
		return;
	}
	
	LXMatrix MatrixMeshWCS = MatrixWCSParent * InMesh->GetMatrix();
	
	// Primitives
	for (const unique_ptr<LXPrimitiveInstance>& PrimitiveInstance : InMesh->GetPrimitives())
	{
		
		if (PrimitiveInstance->Matrix)
		{
			CHK(InMesh->GetMatrix().IsIdentity()); // In case of, integrate below
			MatrixMeshWCS = MatrixWCSParent * *PrimitiveInstance->Matrix;
		}

		LXBBox BBoxWorld = PrimitiveInstance->Primitive->GetBBoxLocal();

		BBoxWorld.ExtendZ(_ExtendZ);

		MatrixMeshWCS.LocalToParent(BBoxWorld);

		// Instance position are in world
		for (const vec3f& Position : _ArrayInstancePosition)
		{
			// TODO : Use a real Position "matrix" transformation
			BBoxWorld.Add(Position);
		}
		
		OutWorldPrimitives.push_back(LXWorldPrimitive(PrimitiveInstance.get(), MatrixMeshWCS, BBoxWorld));
	}

	// Mesh Child
	for (LXMesh* Child: InMesh->GetChild())
	{
		GetAllPrimitives(Child, OutWorldPrimitives, MatrixMeshWCS);
	}
}

void LXActorMesh::OnInvalidateMatrixWCS()
{
	_bValidWorldPrimitives = false;
}