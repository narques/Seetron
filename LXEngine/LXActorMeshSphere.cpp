//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "LXActorMeshSphere.h"
#include "LXAsset.h"
#include "LXEngine.h"
#include "LXMesh.h"
#include "LXMeshFactory.h"
#include "LXMemory.h" // --- Must be the last included ---

LXActorMeshSphere::LXActorMeshSphere(LXProject* pDocument) :LXActorMesh(pDocument)
{
	SetName(L"Sphere");

	Mesh = GetMeshFactory()->CreateSphere(50.f);

	LXProperty::SetCurrentGroup(L"Sphere");
	
	DefineProperty("Slices", &Slices, 1, 64);
	DefineProperty("Stacks", &Stacks, 1, 64);

	LXPropertyAssetPtr* pPropMaterial = DefinePropertyAsset(L"Material", LXPropertyID::PRIMITIVE_MATERIAL, (std::shared_ptr<LXAsset>*)&_material);
	pPropMaterial->SetName(L"Material");
	pPropMaterial->SetLambdaOnChange([this](LXPropertyAssetPtr* PropertyAsset)
	{
		if (LXAsset* Asset = PropertyAsset->GetValue().get())
		{
			LXString Key = PropertyAsset->GetValue()->GetRelativeFilename();
			Mesh->SetMaterial(Key);
		}
	});
}

LXActorMeshSphere::~LXActorMeshSphere()
{
}

void LXActorMeshSphere::OnPropertyChanged(LXProperty* Property)
{
	__super::OnPropertyChanged(Property);

	if (Property->GetID() == LXPropertyID::PRIMITIVE_MATERIAL)
	{
		LXPropertyAssetPtr* PropertyAsset = static_cast<LXPropertyAssetPtr*>(Property);
		LXString Key = PropertyAsset->GetValue()->GetRelativeFilename();
		Mesh->SetMaterial(Key);
	}
}

//------------------------------------------------------------------------------------------------------

LXActorMeshCylinder::LXActorMeshCylinder(LXProject* pDocument) :LXActorMesh(pDocument)
{
	SetName(L"Cylinder");

	Mesh = GetMeshFactory()->CreateCylinder(50.f, 100.f);

	LXProperty::SetCurrentGroup(L"Cylinder");

	DefineProperty("Slices", &Slices, 1, 64);
	DefineProperty("Stacks", &Stacks, 1, 64);

	LXPropertyAssetPtr* pPropMaterial = DefinePropertyAsset(L"Material", LXPropertyID::PRIMITIVE_MATERIAL, (std::shared_ptr<LXAsset>*)&_material);
	pPropMaterial->SetName(L"Material");
	pPropMaterial->SetLambdaOnChange([this](LXPropertyAssetPtr* PropertyAsset)
	{
		if (LXAsset* Asset = PropertyAsset->GetValue().get())
		{
			LXString Key = PropertyAsset->GetValue()->GetRelativeFilename();
			Mesh->SetMaterial(Key);
		}
	});
}

LXActorMeshCylinder::~LXActorMeshCylinder()
{
}

void LXActorMeshCylinder::OnPropertyChanged(LXProperty* Property)
{
	__super::OnPropertyChanged(Property);

	if (Property->GetID() == LXPropertyID::PRIMITIVE_MATERIAL)
	{
		LXPropertyAssetPtr* PropertyAsset = static_cast<LXPropertyAssetPtr*>(Property);
		LXString Key = PropertyAsset->GetValue()->GetRelativeFilename();
		Mesh->SetMaterial(Key);
	}
}

//------------------------------------------------------------------------------------------------------

LXActorMeshCone::LXActorMeshCone():LXActorMesh(GetEngine().GetProject())
{
	SetName(L"Cone");

	Mesh = GetMeshFactory()->CreateCone(50.f, 100.f);
	
	LXProperty::SetCurrentGroup(L"Cone");

	DefineProperty("Slices", &Slices, 1, 64);
	DefineProperty("Stacks", &Stacks, 1, 64);

	LXPropertyAssetPtr* pPropMaterial = DefinePropertyAsset(L"Material", LXPropertyID::PRIMITIVE_MATERIAL, (std::shared_ptr<LXAsset>*)&_material);
	pPropMaterial->SetName(L"Material");
	pPropMaterial->SetLambdaOnChange([this](LXPropertyAssetPtr* PropertyAsset)
	{
		if (LXAsset* Asset = PropertyAsset->GetValue().get())
		{
			LXString Key = PropertyAsset->GetValue()->GetRelativeFilename();
			Mesh->SetMaterial(Key);
		}
	});
}

LXActorMeshCone::~LXActorMeshCone()
{
}

void LXActorMeshCone::OnPropertyChanged(LXProperty* Property)
{
	__super::OnPropertyChanged(Property);

	if (Property->GetID() == LXPropertyID::PRIMITIVE_MATERIAL)
	{
		LXPropertyAssetPtr* PropertyAsset = static_cast<LXPropertyAssetPtr*>(Property);
		LXString Key = PropertyAsset->GetValue()->GetRelativeFilename();
		Mesh->SetMaterial(Key);
	}
}
