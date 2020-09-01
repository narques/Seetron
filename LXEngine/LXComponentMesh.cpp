//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "StdAfx.h"
#include "LXComponentMesh.h"

// Seetron
#include "LXAssetMesh.h"
#include "LXCore.h"
#include "LXMesh.h"

LXComponentMesh::LXComponentMesh(LXActor* actor) :
	LXComponent(actor)
{
	LX_COUNTSCOPEINC(LXComponentMesh);
	_nCID |= LX_NODETYPE_MESH;
	SetName(L"ComponentMesh");
	_mesh = make_shared<LXMesh>();
	DefineProperties();
}

LXComponentMesh::~LXComponentMesh(void)
{
	LX_COUNTSCOPEDEC(LXComponentMesh);
}

void LXComponentMesh::DefineProperties()
{
	// --------------------------------------------------------------------------------------------------------------
	LXProperty::SetCurrentGroup(L"ComponentMesh");
	// --------------------------------------------------------------------------------------------------------------

	//CastShadows
	DefinePropertyBool(L"CastShadows", GetAutomaticPropertyID(), &_castShadows);

	//Asset
	auto PropAssetMesh = DefineProperty(L"AssetMesh", (shared_ptr<LXAsset>*) & _assetMesh);
	PropAssetMesh->SetLambdaOnChange([this](LXPropertyAssetPtr* Property)
	{
		UpdateAssetMeshCallbacks();
		UpdateMesh();
	});
}

void LXComponentMesh::UpdateMesh()
{
	if (_assetMesh)
	{
		if (_mesh != _assetMesh->GetMesh())
		{
			_mesh = _assetMesh->GetMesh();
		}
	}
}

void LXComponentMesh::UpdateAssetMeshCallbacks()
{
	if (_assetMesh != nullptr)
	{
		_assetMesh->RegisterCB(this, L"TransformationChanged", [this](LXSmartObject* smartObject)
		{
			InvalidateRenderState();
		});

		_assetMesh->RegisterCB(this, L"MeshesBoundingBoxInvalidated", [this](LXSmartObject* smartObject)
		{
			InvalidateBounds();
		});

		_assetMesh->RegisterCB(this, L"VisibiltyChanged", [this](LXSmartObject* smartObject)
		{
			InvalidateRenderState();
		});
	}
}

void LXComponentMesh::OnPropertyChanged(LXProperty* property)
{
	__super::OnPropertyChanged(property);

	switch (property->GetID())
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

void LXComponentMesh::SetMesh(shared_ptr<LXMesh>& mesh)
{
	CHK(_mesh == nullptr);
	_mesh = mesh;
	InvalidateBounds();
}

void LXComponentMesh::SetAssetMesh(shared_ptr<LXAssetMesh>& assetMesh)
{
	CHK(_assetMesh == nullptr);
	_assetMesh = assetMesh;
	InvalidateBounds();
	UpdateAssetMeshCallbacks();
	UpdateMesh();
}

void LXComponentMesh::ComputeBBoxLocal()
{
	if (_BBoxLocal.IsValid())
		return;

	// Reset the BBox
	_BBoxLocal.Reset();

	if (_mesh)
	{
		const LXBBox& BBoxMesh = _mesh->GetBounds();

		if (BBoxMesh.IsValid())
			_BBoxLocal.Add(BBoxMesh);
	}

	if (!_BBoxLocal.IsValid())
	{
		LogD(ComponentMesh, L"Set the default BBox size for %s", GetName().GetBuffer());
		_BBoxLocal.Add(LX_VEC3F_XYZ_50);
		_BBoxLocal.Add(LX_VEC3F_NXYZ_50);
	}
}

void LXComponentMesh::AddPrimitive(const shared_ptr<LXPrimitive>& Primitive, const LXMatrix* InMatrix, const LXMaterialBase* InMaterial)
{
	// If no mesh create a new one : TODO in Constructor
	if (!_mesh)
	{
		_mesh = make_shared<LXMesh>();
	}

	_mesh->AddPrimitive(Primitive, InMatrix, InMaterial);
	InvalidateBounds();
	InvalidateRenderState();
}

void LXComponentMesh::ReleaseAllPrimitives()
{
	_mesh->RemoveAllPrimitives();
	InvalidateWorldPrimitives();
	InvalidateBounds();
	InvalidateRenderState();
}

void LXComponentMesh::InvalidateWorldPrimitives()
{
	ReleaseRenderData(ERenderClusterRole::All);
}

const TWorldPrimitives& LXComponentMesh::GetAllPrimitives()
{
	static TWorldPrimitives empty;

	if (_renderData)
		return _renderData->GetPrimitives();
	else
		return empty;
}
