//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "StdAfx.h"
#include "LXCommandDeleteActor.h"
#include "LXActor.h"
#include "LXEngine.h"
#include "LXProject.h"
#include "LXAssetManager.h"
#include "LXAnimationManager.h"
#include "LXKey.h"
#include "LXMemory.h" // --- Must be the last included ---

LXCommandDeleteActor::LXCommandDeleteActor(const SetActors& setActors)
{
	CHK(setActors.size() != 0);
	_setActors = setActors;
}

LXCommandDeleteActor::~LXCommandDeleteActor(void)
{
	for (SetActors::iterator It = _setActors.begin(); It != _setActors.end(); It++)
	{
		if (!(*It)->IsSystem())
			(*It)->MarkForDelete();
		else
			CHK(0);
	}
}

bool LXCommandDeleteActor::Do()
{
	for (SetActors::iterator It=_setActors.begin(); It!=_setActors.end(); It++)
	{
		if (!(*It)->IsSystem())
		{
			LXActor* pActor = *It;
			pActor->GetParent()->RemoveChild(pActor);
			pActor->OnTrashed();
		}
		else
			CHK(0);
	}

	return true;
}

bool LXCommandDeleteActor::Undo()
{
	for (SetActors::iterator It=_setActors.begin(); It!=_setActors.end(); It++)
	{
		LXActor* pActor = *It;
		LXActor* pGroup = pActor->GetPreviousParent();
		pActor->SetParent(NULL); // Avoid CHK in LXActor::AddChild
		pGroup->AddChild(pActor);
		pActor->OnRecycled();
	}
	return true;
}

LXCommandDeleteMaterial::LXCommandDeleteMaterial(const SetMaterials& setMaterials)
{
	CHK(setMaterials.size() != 0);
	
	for_each(setMaterials.begin(), setMaterials.end(), [](LXMaterial* pMaterial){
		CHK(!pMaterial->IsSystem());		
	});

	_setMaterials = setMaterials;
}

LXCommandDeleteMaterial::~LXCommandDeleteMaterial(void)
{
	for (SetMaterials::iterator It = _setMaterials.begin(); It != _setMaterials.end(); It++)
	{
		if (!(*It)->IsSystem())
			delete *It;
		else
			CHK(0);
	}
}

bool LXCommandDeleteMaterial::Do()
{
	for (SetMaterials::iterator It=_setMaterials.begin(); It!=_setMaterials.end(); It++)
	{
		LXMaterial* pMaterial = *It;
		CHK(0); // TODO:
	}

	return true;
}

bool LXCommandDeleteMaterial::Undo()
{
	for (SetMaterials::iterator It=_setMaterials.begin(); It!=_setMaterials.end(); It++)
	{
		LXMaterial* pMaterial = *It;
		CHK(0); // TODO:
	}
	return true;
}

LXCommandDeleteKey::LXCommandDeleteKey(const SetKeys& setKeys)
{
	CHK(setKeys.size() != 0);

	for_each(setKeys.begin(), setKeys.end(), [](LXKey* pKey)
	{
		CHK(!pKey->IsSystem());
	});

	_setKeys = setKeys;
}

LXCommandDeleteKey::~LXCommandDeleteKey(void)
{
	for (SetKeys::iterator It = _setKeys.begin(); It != _setKeys.end(); It++)
	{
		if (!(*It)->IsSystem())
			delete *It;
		else
			CHK(0);
	}
}

bool LXCommandDeleteKey::Do()
{
	for (SetKeys::iterator It = _setKeys.begin(); It != _setKeys.end(); It++)
	{
		LXKey* pKey = *It;
		GetEngine().GetProject()->GetAnimationManager().Remove(pKey);
	}

	return true;
}

bool LXCommandDeleteKey::Undo()
{
	for (SetKeys::iterator It = _setKeys.begin(); It != _setKeys.end(); It++)
	{
		LXKey* pKey = *It;
		GetEngine().GetProject()->GetAnimationManager().Add(pKey);
	}
	return true;
}
