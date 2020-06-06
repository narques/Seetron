//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "StdAfx.h"
#include "LXActorMesh.h"
#include "LXAssetMesh.h"
#include "LXCore.h"
#include "LXMesh.h"
#include "LXMemory.h" // --- Must be the last included ---

LXActorMesh::LXActorMesh():
LXActor(GetCore().GetProject())
{
	LX_COUNTSCOPEINC(LXActorMesh);
	_nCID |= LX_NODETYPE_MESH;
	SetName(L"ActorMesh");
	DefineProperties();
}

LXActorMesh::LXActorMesh(LXProject* pDocument):
LXActor(pDocument)
{
	LX_COUNTSCOPEINC(LXActorMesh);
	_nCID |= LX_NODETYPE_MESH;
	SetName(L"Mesh");
	DefineProperties();
}

LXActorMesh::~LXActorMesh(void)
{
	LX_COUNTSCOPEDEC(LXActorMesh);
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
	
	//CastShadows
	DefinePropertyBool(L"CastShadows", GetAutomaticPropertyID(), &_bCastShadows);

	//Asset
	auto PropAssetMesh = DefineProperty(L"AssetMesh", (shared_ptr<LXAsset>*)&_AssetMesh);
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

void LXActorMesh::SetAssetMesh(shared_ptr<LXAssetMesh>& AssetMesh)
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
	ReleaseRenderData(ERenderClusterRole::All);
}

const TWorldPrimitives& LXActorMesh::GetAllPrimitives()
{
	static TWorldPrimitives empty;

	if (_renderData)
		return _renderData->GetPrimitives();
	else
		return empty;
}

