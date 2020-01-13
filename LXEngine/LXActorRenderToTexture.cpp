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
#include "LXMesh.h"
#include "LXMeshFactory.h"
#include "LXTexture.h"
#include "LXMemory.h" // --- Must be the last included ---

LXActorRenderToTexture::LXActorRenderToTexture(LXProject* project):
	LXActorMesh(project)
{
	SetName(L"RenderToTexture");
	_nCID |= LX_NODETYPE_RENDERTOTTEXTURE;
	
	Mesh = GetMeshFactory()->CreateSSTriangle();

	LXProperty::SetCurrentGroup(L"RenderToTexture");

	LXPropertyAssetPtr* pPropTexture = DefinePropertyAsset(L"Texture", GetAutomaticPropertyID(), (LXAsset**)&_texture);
	pPropTexture->SetName(L"Texture");
	pPropTexture->SetLambdaOnChange([this](LXPropertyAssetPtr* PropertyAsset)
	{
		Mesh->SetMaterial(_texture->GetMaterial());
	});
}

LXActorRenderToTexture::~LXActorRenderToTexture()
{

}
