//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "LXActorMeshPlane.h"
#include "LXPrimitive.h"
#include "LXPrimitiveInstance.h"
#include "LXMesh.h"
#include "LXCore.h"
#include "LXPrimitiveFactory.h"
#include "LXMemory.h" // --- Must be the last included ---

LXActorMeshPlane::LXActorMeshPlane(LXProject* pDocument):
LXActorMesh(pDocument)
{
	SetName(L"Plane");
	Mesh = make_shared<LXMesh>(nullptr);

	LXProperty::SetCurrentGroup(L"Plane");

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
	
	const shared_ptr<LXPrimitive>& Primitive = GetPrimitiveFactory()->CreateQuadXY(1000.f, 1000.f, true);
	Primitive->SetPersistent(false);
	Primitive->ComputeTangents();
	AddPrimitive(Primitive);
}

LXActorMeshPlane::~LXActorMeshPlane()
{
}

void LXActorMeshPlane::OnPropertyChanged(LXProperty* Property)
{
	__super::OnPropertyChanged(Property);
// 	if ((pProperty->GetID() == propIDWidth) ||
// 		(pProperty->GetID() == propIDLength))
// 	{
// 		LXPrimitive* pPrimitive = GetDrawables()[0]->Primitive;
// 
// 		float w = _width * 0.5f;
// 		float h = _length * 0.5f;
// 
// 		const vec3f v0(-w, +h, 0.f);
// 		const vec3f v1(-w, -h, 0.f);
// 		const vec3f v2(+w, +h, 0.f);
// 		const vec3f v3(+w, -h, 0.f);
// 		
// 		pPrimitive->GetArrayPositions()[0].Set(v0);
// 		pPrimitive->GetArrayPositions()[1].Set(v1); 
// 		pPrimitive->GetArrayPositions()[2].Set(v2);
// 		pPrimitive->GetArrayPositions()[3].Set(v3);
// 		
// 		//pPrimitive->BanchUpdatePositions();
// 		
// 		InvalidatePrimitiveBounds(true);
// 	}
// 	else if (pProperty->GetID() == propIDMaterial)
// 	{
// 		LXPrimitive* pPrimitive = GetDrawables()[0]->Primitive;
// 		pPrimitive->SetMaterial(_material);
// 	}
}

