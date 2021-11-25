//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "pch.h"
#include "LXPropertyAsset.h"

// Seetron
#include "LXAsset.h"
#include "LXAssetManager.h"
#include "LXCommand.h"
#include "LXCommandModifyProperty.h"
#include "LXEngine.h"
#include "LXGraphMaterial.h"
#include "LXLogger.h"
#include "LXMSXMLNode.h"
#include "LXMaterial.h"
#include "LXMaterialNode.h"
#include "LXMatrix.h"
#include "LXNode.h"
#include "LXObjectFactory.h"
#include "LXProject.h"
#include "LXSettings.h"

LXPropertyAsset* LXPropertyAsset::Create(LXSmartObject* owner, const LXString& name, const LXPropertyID& PID, std::shared_ptr<LXAsset>* var)
{
	LXPropertyAsset* pProperty = new LXPropertyAsset();
	pProperty->SetID(PID);
	pProperty->SetName(name);
	pProperty->SetLabel(name);
	pProperty->_Var = var;
	owner->AddProperty(pProperty);
	return pProperty;
}

LXPropertyAsset* LXPropertyAsset::Create(LXSmartObject* owner, const LXString& name, std::shared_ptr<LXAsset>* var)
{
	return LXPropertyAsset::Create(owner, name, GetAutomaticPropertyID(), var);
}

LXProperty* LXPropertyAsset::Create(const LXString& name, void* var) const
{
	LXPropertyAsset* property = new LXPropertyAsset();
	property->SetName(name);
	property->SetID(GetAutomaticPropertyID());
	property->_Var = (LXAssetPtr*)var;
	return property;
}

LXPropertyAsset* LXPropertyAsset::CreateUserProperty(LXSmartObject* owner, const LXString& name, const LXAssetPtr& var)
{
	LXVariantT<LXAssetPtr>* variant = new LXVariantT<LXAssetPtr>(var);
	owner->AddVariant(variant);
	LXPropertyAsset* property = Create(owner, name, variant->GetValuePtr());
	property->SetUserProperty(true);
	return property;
}

LXPropertyAsset::LXPropertyAsset():LXProperty(EPropertyType::AssetPtr)
{
}

void LXPropertyAsset::LoadXML(const TLoadContext& loadContext)
{
	GetValueFromXML2(loadContext);
}

void LXPropertyAsset::SaveXML(const TSaveContext& saveContext)
{

}

void LXPropertyAsset::GetValueFromXML2(const TLoadContext& loadContext)
{
	const LXMSXMLNode& node = loadContext.node;
	LXString strFilename;
	LXAssetPtr value;
	LXPropertyHelper::GetValueFromXML(node, strFilename);
	if (!strFilename.IsEmpty())
	{
		LXProject* Project = GetEngine().GetProject();
		CHK(Project);
		if (Project)
		{
			LXAssetManager& mm = Project->GetAssetManager();
			value = mm.GetAsset(strFilename);
			CHK(value);
		}
	}

	SetValue(value, false);
}

void LXPropertyAsset::SetValue(const std::shared_ptr<LXAsset> value, bool InvokeOnProperyChanged)
{
	// If no change, return.
// 	if (value == GetValue())
// 	{
// 		return;
// 	}

	ValueChanging.Invoke(this);

	//T clampedValue = CheckRange(value);

	//
	//LoadValue(value);
	//
	LXAssetPtr asset = std::dynamic_pointer_cast<LXAsset>(value);
	if (asset)
	{
		asset->Load();
	}
	//value->Load();

	*_Var = value;

	// Event (By Property)
	ValueChanged.Invoke(this);

	// Event (By Object & By App)
	if (InvokeOnProperyChanged)
		_Owner->OnPropertyChanged(this);
}

void LXPropertyAsset::SaveXML2(const TSaveContext& saveContext, const LXString& strXMLName, const LXAssetPtr& v)
{
	LXString strFilename;
	if (v)
		strFilename = v->GetRelativeFilename();
	LXPropertyHelper::SaveXML(saveContext, strXMLName, strFilename);
}

