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

	_vectorPrimitives.clear();

	for (LXMesh* Mesh : _Children)
	{
		delete Mesh;
	}
}

bool LXMesh::OnSaveChild(const TSaveContext& saveContext) const
{
	// Save primitives
	for (VectorPrimitiveInstances::const_iterator It = _vectorPrimitives.begin(); It != _vectorPrimitives.end(); It++)
	{
		const unique_ptr<LXPrimitiveInstance>& PrimitiveInstance = *It;
 
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

void LXMesh::AddPrimitive(const shared_ptr<LXPrimitive>& Primitive, LXMatrix* InMatrix /*= nullptr*/, LXMaterial* InMaterial /*= nullptr */)
{
	LX_CHK_RET(Primitive);
	
	LXMatrix* Matrix = nullptr;
	if (InMatrix)
		Matrix = new LXMatrix(*InMatrix);

	_vectorPrimitives.push_back(make_unique<LXPrimitiveInstance>(Primitive, Matrix, InMaterial));
	
	InvalidateBounds();
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
	for (const auto& PrimitiveInstance : _vectorPrimitives)
	{
		PrimitiveInstance->Primitive->Release();
	}
	_vectorPrimitives.clear();
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
	for (const unique_ptr<LXPrimitiveInstance>& PrimitiveInstance : _vectorPrimitives)
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

void LXMesh::SetMaterial(const LXString& Key)
{
	for (const unique_ptr<LXPrimitiveInstance>& PrimitiveInstance : _vectorPrimitives)
	{
		PrimitiveInstance->Primitive->SetMaterial(Key);
	}
}

void LXMesh::SetMaterial(LXMaterial* material)
{
	for (const unique_ptr<LXPrimitiveInstance>& PrimitiveInstance : _vectorPrimitives)
	{
		PrimitiveInstance->Primitive->SetMaterial(material);
	}
}

