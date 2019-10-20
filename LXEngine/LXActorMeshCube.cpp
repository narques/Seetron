//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "LXCore.h"
#include "LXActorMeshCube.h"
#include "LXPrimitive.h"
#include "LXMesh.h"
#include "LXPrimitiveFactory.h"
#include "LXMemory.h" // --- Must be the last included ---

LXActorMeshCube::LXActorMeshCube(LXProject* pDocument):LXActorMesh(pDocument)
{
	SetName(L"Cube");

	Mesh = make_shared<LXMesh>(nullptr);

	LXProperty::SetCurrentGroup(L"Cube");
	
	LXPropertyAssetPtr* pPropMaterial = DefinePropertyAsset(L"Material", LXPropertyID::PRIMITIVE_MATERIAL, (LXAsset**)&_Material);
	pPropMaterial->SetName(L"Material");
	pPropMaterial->SetLambdaOnChange([this](LXPropertyAssetPtr* PropertyAsset)
	{
		if (LXAsset* Asset = PropertyAsset->GetValue())
		{
			LXString Key = PropertyAsset->GetValue()->GetRelativeFilename();
			Mesh->SetMaterial(Key);
		}
	});
		
	const shared_ptr<LXPrimitive>& Primitive = GetPrimitiveFactory()->CreateCube(100.f, 100.f, 100.f);
	Primitive->SetPersistent(false);
	Primitive->ComputeNormals();
	Primitive->ComputeTangents();
	AddPrimitive(Primitive);
}

LXActorMeshCube::~LXActorMeshCube()
{
}
