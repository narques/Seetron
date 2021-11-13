//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "pch.h"
#include "LXActorMeshPlane.h"
#include "LXAsset.h"
#include "LXEngine.h"
#include "LXMesh.h"
#include "LXMeshFactory.h"

LXActorMeshPlane::LXActorMeshPlane(LXProject* pDocument):
LXActorMesh(pDocument)
{
	SetName(L"Plane");
	Mesh = GetMeshFactory()->CreateQuadXY(1000.f, 1000.f);

	LXProperty::SetCurrentGroup(L"Plane");

	LXPropertyAssetPtr* pPropMaterial = DefinePropertyAsset(L"Material", LXPropertyID::PRIMITIVE_MATERIAL, (std::shared_ptr<LXAsset>*)&_Material);
	pPropMaterial->SetName(L"Material");
	pPropMaterial->ValueChanged.AttachMemberLambda([this](LXProperty* property)
	{
		LXPropertyAssetPtr* PropertyAsset = static_cast<LXPropertyAssetPtr*>(property);
		if (LXAsset* Asset = PropertyAsset->GetValue().get())
		{
			LXString Key = PropertyAsset->GetValue()->GetRelativeFilename();
			Mesh->SetMaterial(Key);
		}
	});
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

