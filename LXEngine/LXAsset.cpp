//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "LXAsset.h"
#include "LXEngine.h"
#include "LXProject.h"
#include "LXSettings.h"
#include "LXLogger.h"

LXAsset::LXAsset()
{
	DefineProperties();
}

LXAsset::~LXAsset()
{
}

bool LXAsset::CanBeSaved()
{
	return (State == EResourceState::LXResourceState_Loaded);
}

bool LXAsset::CanBeReloaded()
{
	return true;
}

bool LXAsset::Save()
{
	if (!GetPersistent())
		return false;

	if (!CanBeSaved())
	{
		LogE(Resource, L"Unable to save. Resource %s not loaded", _Name);
		return false;
	}

	TSaveContext saveContext;
	saveContext.bSaveChilds = true;// bSaveChilds;
	saveContext.bSaveSystem = false;// bSaveSystem;
	saveContext.Owner = this;

	// XML file
	FILE* pFile = NULL;
	errno_t err = _wfopen_s(&pFile, _filepath, L"wt");
	CHK(pFile && !err);
	if (!pFile || err)
		return false;
	fwprintf(pFile, L"<?xml version=\"1.0\" encoding=\"utf-8\"?>\n");
	saveContext.pXMLFile = pFile;
	LXSmartObject::Save(saveContext);
	fclose(pFile);
	_bNeedSave = false;
	return true;
}

LXString LXAsset::GetRelativeFilename() const
{
	if (Owner == EResourceOwner::LXResourceOwner_Engine)
	{
		return GetSettings().GetDataFolder().GetRelativeFilepath(_filepath);
	}
	else if (Owner == EResourceOwner::LXResourceOwner_Project)
	{
		return GetEngine().GetProject()->GetAssetFolder().GetRelativeFilepath(_filepath);
	}
	else
	{
		CHK(0);
		return L"";
	}
}

ListProperties LXAsset::GetProperties() const
{
	ListProperties listProperties =  __super::GetProperties();
	
	if (State == EResourceState::LXResourceState_Unloaded)
	{
		(const_cast<LXAsset*>(this))->Load();
	}

	// Load failed
	if (State == EResourceState::LXResourceState_Unloaded)
	{
		LogE(Asset, L"Failed to retrieve properties: Asset %s not loaded", _filepath.GetBuffer());
		ListProperties EmptyListProperties;
		return EmptyListProperties;
	}

	return listProperties;
}

void LXAsset::DefineProperties()
{
	//--------------------------------------------------------------------------------------------------------------------------------
	LXProperty::SetCurrentGroup(L"Resource");
	//--------------------------------------------------------------------------------------------------------------------------------

	auto p = DefinePropertyString(L"Filepath", GetAutomaticPropertyID(), &_filepath);
	p->SetReadOnly(true);
	p->SetPersistent(false);

	auto PropertyState = DefinePropertyEnum("State", GetAutomaticPropertyID(), (uint*)&State);
	PropertyState->AddChoice(L"Unloaded", (uint)EResourceState::LXResourceState_Unloaded);
	PropertyState->AddChoice(L"Loaded", (uint)EResourceState::LXResourceState_Loaded);
	PropertyState->SetReadOnly(true);
	PropertyState->SetPersistent(false);
}