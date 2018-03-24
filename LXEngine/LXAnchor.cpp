//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "StdAfx.h"
#include "LXAnchor.h"
#include "LXActor.h"
#include "LXProject.h"
#include "LXSettings.h"
#include "LXMemory.h" // --- Must be the last included ---

#if LX_ANCHOR

LXAnchor::LXAnchor(LXProject* pDocument):LXActorMesh(pDocument)
{
	_nCID |= LX_NODETYPE_ANCHOR;
	
	SetName(L"Anchor");
	//SetBillboard(true);
		
	_material = pDocument->GetMaterialManager().GetMaterial(L"sysAnchor");
			
	LXPrimitive* pPrim = LXPrimitive::CreatePoint();
	
	pPrim->SetMaterial(_material);
	AddPrimitive(pPrim);
}

LXAnchor::~LXAnchor()
{
}

void LXAnchor::SetOwner(LXActor* owner)
{
	CHK(!_owner);
	_owner = owner;
}

#endif