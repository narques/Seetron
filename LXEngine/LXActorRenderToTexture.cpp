//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "LXActorRenderToTexture.h"
#include "LXCore.h"
#include "LXMaterial.h"
#include "LXMesh.h"
#include "LXMeshFactory.h"
#include "LXRenderer.h"
#include "LXTexture.h"
#include "LXMemory.h" // --- Must be the last included ---

LXActorRenderToTexture::LXActorRenderToTexture(LXProject* project):
	LXActorMesh(project)
{
	SetName(L"RenderToTexture");
	_nCID |= LX_NODETYPE_RENDERTOTTEXTURE;
	
	Mesh = GetMeshFactory()->CreateSSTriangle();

	LXProperty::SetCurrentGroup(L"RenderToTexture");

	LXPropertyAssetPtr* propertyMaterial = DefineProperty("Material", (LXAsset**)&_material);
	propertyMaterial->SetName(L"Material");
	propertyMaterial->ValueChanged.AttachMemberLambda([this](LXProperty*) { MaterialChanged(); });
	
	LXPropertyAssetPtr* pPropTexture = DefinePropertyAsset(L"Texture", GetAutomaticPropertyID(), (LXAsset**)&_texture);
	pPropTexture->SetName(L"Texture");
}

LXActorRenderToTexture::~LXActorRenderToTexture()
{
}

void LXActorRenderToTexture::SetTexture(LXTexture* texture)
{
	_texture = texture;
}

void LXActorRenderToTexture::SetMaterial(LXMaterial* Material)
{
	(static_cast<LXPropertyAssetPtr*>(GetProperty(L"Material")))->SetValue(Material);
}

void LXActorRenderToTexture::CopyDeviceToBitmap()
{
	if (_texture)
		_texture->CopyDeviceToBitmap();
}

void LXActorRenderToTexture::MaterialChanged()
{
	Mesh->SetMaterial(_material);
}
