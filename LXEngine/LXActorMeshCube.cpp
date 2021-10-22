//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "LXActorMeshCube.h"
#include "LXAsset.h"
#include "LXEngine.h"
#include "LXMesh.h"
#include "LXMeshFactory.h"

LXActorMeshCube::LXActorMeshCube(LXProject* pDocument):LXActorMesh(pDocument)
{
	SetName(L"Cube");

	Mesh = GetMeshFactory()->CreateCube(100.f, 100.f, 100.f);

	LXProperty::SetCurrentGroup(L"Cube");
	   	
	LXPropertyAssetPtr* pPropMaterial = DefinePropertyAsset(L"Material", LXPropertyID::PRIMITIVE_MATERIAL, (std::shared_ptr<LXAsset>*)&_Material);
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

LXActorMeshCube::~LXActorMeshCube()
{
}
