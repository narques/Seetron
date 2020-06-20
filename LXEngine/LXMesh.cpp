//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "LXAssetManager.h"
#include "LXAssetMesh.h"
#include "LXCore.h"
#include "LXMSXMLNode.h"
#include "LXMaterial.h"
#include "LXMesh.h"
#include "LXPrimitive.h"
#include "LXPrimitiveInstance.h"
#include "LXProject.h"
#include "LXStatistic.h"
#include "LXMemory.h" // --- Must be the last included ---

LXMesh::LXMesh()
{
	LX_COUNTSCOPEINC(LXMesh)
}

LXMesh::LXMesh(LXAssetMesh* InOwner):_Owner(InOwner)
{
	LX_COUNTSCOPEINC(LXMesh)

	// Properties

	// --------------------------------------------------------------------------------------------------------------
	LXProperty::SetCurrentGroup(L"Bounds");
	// --------------------------------------------------------------------------------------------------------------

	LXPropertyFloat* pPropSizeX = DefinePropertyFloat(L"SizeX", LXPropertyID::SIZEX, NULL);
	pPropSizeX->SetLambdaOnGet([this]() { return GetBounds().GetSizeX(); });
	pPropSizeX->SetPersistent(false);
	pPropSizeX->SetReadOnly(true);

	LXPropertyFloat* pPropSizeY = DefinePropertyFloat(L"SizeY", LXPropertyID::SIZEY, NULL);
	pPropSizeY->SetLambdaOnGet([this]() { return GetBounds().GetSizeY(); });
	pPropSizeY->SetPersistent(false);
	pPropSizeY->SetReadOnly(true);

	LXPropertyFloat* pPropSizeZ = DefinePropertyFloat(L"SizeZ", LXPropertyID::SIZEZ, NULL);
	pPropSizeZ->SetLambdaOnGet([this]() { return GetBounds().GetSizeZ(); });
	pPropSizeZ->SetPersistent(false);
	pPropSizeZ->SetReadOnly(true);

	// --------------------------------------------------------------------------------------------------------------
	LXProperty::SetCurrentGroup(L"Transformation");
	// --------------------------------------------------------------------------------------------------------------

	_Transformation.DefineProperties(this);
	_Transformation.OnChange([this]()
	{
		if (_Owner != nullptr)
		{
			_Owner->OnMeshesdTransformationChanged();
			InvalidateBounds();
		}
	});

	LXPropertyBool* property = DefineProperty(L"Visible", &_visible);
	property->SetLambdaOnChange([this](LXProperty* pProperty)
	{
		if (_Owner != nullptr)
		{
			_Owner->OnMeshVisibilityChanged();
		}
	});
}

LXMesh::~LXMesh()
{
	LX_COUNTSCOPEDEC(LXMesh)

	for (const shared_ptr<LXPrimitiveInstance>& PrimitiveInstance : _vectorPrimitives)
	{
		CHK(PrimitiveInstance->Owners.size() == 0);
	}
	
	_vectorPrimitives.clear();

	for (LXMesh* Mesh : _Children)
	{
		delete Mesh;
	}
}

bool LXMesh::OnSaveChild(const TSaveContext& saveContext) const
{
	// Save primitives
	for (const shared_ptr<LXPrimitiveInstance>& PrimitiveInstance : _vectorPrimitives)
	{
		if (PrimitiveInstance->Primitive->GetPersistent())
		{
			int nGeoId = PrimitiveInstance->Primitive->GetId();
			LXString strMaterialFilename;
			if (PrimitiveInstance->Primitive->GetMaterial())
				strMaterialFilename = PrimitiveInstance->Primitive->GetMaterial()->GetRelativeFilename();
 
			fwprintf(saveContext.pXMLFile, L"%s", GetTab(saveContext.Indent).GetBuffer());
			fwprintf(saveContext.pXMLFile, L"<Geometry Id=\"%i\" Material=\"%s\"/>\n", nGeoId, strMaterialFilename.GetBuffer());
			PrimitiveInstance->Primitive->Save(saveContext);
		}
	}
 
	// Save children
	for (LXMesh* Mesh : _Children)
		Mesh->Save(saveContext);

	return true;
}

bool LXMesh::OnLoadChild(const TLoadContext& loadContext)
{
	const LXString& name = loadContext.node.name();
	bool bRet = false;

	if (name == L"LXMesh")
	{
		LXMesh* Mesh = new LXMesh(_Owner);
		Mesh->Load(loadContext);
		AddChild(Mesh);
		bRet = true;
	}

	if (loadContext.node.name() == L"Geometry")
	{
		// Geometry ID
		int nGeoId = loadContext.node.attrInt(L"Id", -1);
		if (nGeoId != -1)
		{
			LXAssetMesh* AssetMesh = dynamic_cast<LXAssetMesh*>(loadContext.pOwner);
			CHK(AssetMesh);
			
			const MapGeometries& pMapGeometries = AssetMesh->GetPrimitives();
			MapGeometries::const_iterator It = pMapGeometries.find(nGeoId);
			if (It != pMapGeometries.end())
			{
				const shared_ptr<LXPrimitive>& Primitive = It->second;

				if (Primitive->GetArrayPositions().size())
				{
					AddPrimitive(Primitive);
				}

				LXString MaterialName = loadContext.node.attr(L"Material");
				Primitive->SetMaterial(MaterialName);
			}
			else
				CHK(0);
		}

		bRet = true;
	}

	return bRet;
}

void LXMesh::AddChild(LXMesh* Mesh)
{
	CHK(Mesh);
	if (!Mesh)
		return;

	CHK(!Mesh->GetParent());
	Mesh->SetParent(this);
	_Children.push_back(Mesh);
	InvalidateBounds();
	
}

void LXMesh::AddPrimitive(const shared_ptr<LXPrimitive>& Primitive, const LXMatrix* InMatrix /*= nullptr*/, const LXMaterial* InMaterial /*= nullptr */)
{
	LX_CHK_RET(Primitive);
	
	_vectorPrimitives.push_back(make_unique<LXPrimitiveInstance>(Primitive, InMatrix, InMaterial));
	
	InvalidateBounds();
	ComputeMatrixRCS();
}

void LXMesh::RemovePrimitive(LXPrimitive* Primitive)
{
	LX_CHK_RET(Primitive);
	for (auto It = _vectorPrimitives.begin(); It != _vectorPrimitives.end(); It++)
	{
		if ((*It)->Primitive.get() == Primitive)
		{
			_vectorPrimitives.erase(It);
			break;
		}
	}
}

void LXMesh::RemoveAllPrimitives()
{
	_vectorPrimitives.clear();
}

void LXMesh::GetAllPrimitives(VectorPrimitiveInstances& primitives)
{
	for (const shared_ptr<LXPrimitiveInstance>& it : _vectorPrimitives)
	{
		primitives.push_back(it);
	}
	
	for (LXMesh* child : _Children)
	{
		child->GetAllPrimitives(primitives);
	}
}

void LXMesh::ComputeMatrixRCS()
{
	if (_Parent == nullptr)
		ComputeMatrixRCS(nullptr);
}

void LXMesh::ComputeMatrixRCS(const LXMatrix* matrixParentRCS)
{
	LXMatrix matrixMeshRCS = matrixParentRCS ? *matrixParentRCS * GetMatrix() : GetMatrix();

	for (const auto& it : _vectorPrimitives)
	{
		LXPrimitiveInstance* primitiveInstance = &*it;

		if (primitiveInstance->Matrix != nullptr)
		{
			matrixMeshRCS = matrixParentRCS ? *matrixParentRCS * *primitiveInstance->Matrix : *primitiveInstance->Matrix; // * GetMatrix()
		}
		
		LX_SAFE_DELETE(primitiveInstance->MatrixRCS);
		primitiveInstance->MatrixRCS = matrixMeshRCS.IsIdentity() ? nullptr : new LXMatrix(matrixMeshRCS);
	}

	for (LXMesh* child : _Children)
	{
		child->ComputeMatrixRCS(&matrixMeshRCS);
	}
}

const LXBBox& LXMesh::GetBounds()
{
	if (!_BBox.IsValid())
		ComputeBounds();

	return _BBox;
}

void LXMesh::ComputeBounds()
{
	if (_BBox.IsValid())
		return;

	// Reset the BBox
	_BBox.Reset();
	
	// Add Children BBox
	for (LXMesh* MeshChild : _Children)
	{
		_BBox.Add(MeshChild->GetBounds());
	}

	// Add with PrimitiveInstance
	for (const shared_ptr<LXPrimitiveInstance>& PrimitiveInstance : _vectorPrimitives)
	{
		LXPrimitive* pPrimitive = PrimitiveInstance->Primitive.get();

		if (PrimitiveInstance->Matrix)
		{
			LXBBox BBoxPrimitiveInstance = PrimitiveInstance->Primitive->GetBBoxLocal();
			PrimitiveInstance->Matrix->LocalToParent(BBoxPrimitiveInstance);
			_BBox.Add(BBoxPrimitiveInstance);
		}
		else
		{
			_BBox.Add(pPrimitive->GetBBoxLocal());
		}
	}

	if (!_BBox.IsValid())
	{
		// We cannot set arbitrary values (to create a default box of 1m)
		// Because the scale matrix could modify it too much.
		_BBox.Add(LX_VEC3F_NULL);
	}

	GetMatrix().LocalToParent(_BBox);
}

void LXMesh::InvalidateBounds()
{
	_BBox.Invalidate();
	if (_Parent)
	{
		_Parent->InvalidateBounds();
	}
	else if (_Owner != nullptr)
	{
		// Notices the owner only when we are at the root level (no parent). Avoid redundant invalidation notifications.
		_Owner->OnMeshesBoundingBoxInvalidated();
	}
}

void LXMesh::SetMaterial(const LXString& key)
{
	LXAssetManager& rm = GetCore().GetProject()->GetAssetManager();
	shared_ptr<LXMaterial> Material = rm.GetMaterial(key);
	SetMaterial(Material);
}

void LXMesh::SetMaterial(shared_ptr<LXMaterial>& material)
{
	for (const shared_ptr<LXPrimitiveInstance>& PrimitiveInstance : _vectorPrimitives)
	{
		PrimitiveInstance->SetMaterial(material);
	}
}

