//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "pch.h"
#include "LXComponentMesh.h"

// Seetron
#include "LXAssetMesh.h"
#include "LXEngine.h"
#include "LXLogger.h"
#include "LXMesh.h"
#include "LXMeshBase.h"
#include "LXStatistic.h"

LXComponentMesh::LXComponentMesh(LXActor* actor) :
	LXComponent(actor)
{
	LX_COUNTSCOPEINC(LXComponentMesh);
	_nCID |= LX_NODETYPE_MESH;
	SetName(L"ComponentMesh");
	_mesh = std::dynamic_pointer_cast<LXMeshBase>(std::make_shared<LXMesh2>());
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
	auto PropAssetMesh = LXPropertyAsset::Create(this, L"AssetMesh", (std::shared_ptr<LXAsset>*) & _assetMesh);
	PropAssetMesh->ValueChanged.AttachMemberLambda([this](LXProperty* property)
	{
		UpdateAssetMeshCallbacks();
		UpdateMesh();
	});
}

void LXComponentMesh::UpdateMesh()
{
	if (_assetMesh)
	{
		std::shared_ptr<LXMeshBase> meshBase = std::static_pointer_cast<LXMeshBase>(_assetMesh->GetMesh());

		if (_mesh != meshBase)
		{
			_mesh = meshBase;
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

void LXComponentMesh::SetMesh(std::shared_ptr<LXMeshBase>& mesh)
{
	CHK(_mesh == nullptr);
	_mesh = mesh;
	InvalidateBounds();
}

void LXComponentMesh::SetAssetMesh(std::shared_ptr<LXAssetMesh>& assetMesh)
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

void LXComponentMesh::AddPrimitive(const std::shared_ptr<LXPrimitive>& primitive, const LXMatrix* matrix, const std::shared_ptr<LXMaterialBase>& material, int LODIndex)
{
	// If no mesh create a new one : TODO in Constructor
	if (!_mesh)
	{
		_mesh = std::dynamic_pointer_cast<LXMeshBase>(std::make_shared<LXMesh2>());
	}

	_mesh->AddPrimitive(primitive, matrix, material, LODIndex);
	InvalidateBounds();
	InvalidateRenderState();
}

void LXComponentMesh::ReleaseAllPrimitives()
{
	std::shared_ptr<LXMesh> mesh = std::dynamic_pointer_cast<LXMesh>(_mesh);
	if (mesh)
		mesh->RemoveAllPrimitives();
	InvalidateWorldPrimitives();
	InvalidateBounds();
	InvalidateRenderState();
}

void LXComponentMesh::SetLODData(int LODIndex, float maxDistance)
{
	_LODMaxDistance[LODIndex] = maxDistance;
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
