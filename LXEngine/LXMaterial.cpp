//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "StdAfx.h"
#include "LXMaterial.h"

// Seetron
#include "LXAssetManager.h"
#include "LXGraphMaterial.h"
#include "LXGraphTemplate.h"
#include "LXMaterialInstance.h"
#include "LXMaterialNode.h"
#include "LXNode.h"
#include "LXProject.h"
#include "LXRenderer.h"
#include "LXTexture.h"

LXMaterial::LXMaterial(EMaterialType InMaterialType):MaterialType(InMaterialType)
{
	SetName(L"Material");
	GraphMaterial = make_unique<LXGraphMaterial>(this);
	DefineProperties();
}

LXMaterial::~LXMaterial(void)
{
}

void LXMaterial::DefineProperties()
{
	// --------------------------------------------------------------------------------------------------------------
	LXProperty::SetCurrentGroup(L"Material");
	// --------------------------------------------------------------------------------------------------------------
	
	LXPropertyEnum* pPropMaterialType = DefinePropertyEnum(L"Type", GetAutomaticPropertyID(), (uint*)&MaterialType);
	pPropMaterialType->SetReadOnly(true);
	pPropMaterialType->AddChoice(L"Standard", (uint)EMaterialType::MaterialTypeStandard);
	pPropMaterialType->AddChoice(L"ProceduralTexture", (uint)EMaterialType::MaterialTypeTexture);
		
	DefineProperty("TwoSided", &_bTwoSided);

	LXPropertyEnum* PropLightingModel = DefinePropertyEnum(L"LightingModel", GetAutomaticPropertyID(), (uint*)&_LightingModel);
	PropLightingModel->AddChoice(L"Lit", (uint)EMaterialLightingModel::Lit);
	PropLightingModel->AddChoice(L"Unlit", (uint)EMaterialLightingModel::Unlit);
	PropLightingModel->AddChoice(L"Transparent", (uint)EMaterialLightingModel::Transparent);

	DefineProperty("Graph", (LXSmartObject*)GraphMaterial.get());
}

void LXMaterial::GetChildProperties(ListProperties& listProperties) const
{
	__super::GetChildProperties(listProperties);
}

bool LXMaterial::Load()
{
	if (State == EResourceState::LXResourceState_Loaded)
		return true;

	bool Result = false;
	Result =  LoadWithMSXML(_filepath);

	if (Result)
	{
		State = EResourceState::LXResourceState_Loaded;
	}

	return false;
}

bool LXMaterial::Reload()
{
	ReleaseGraph();
	State = LXAsset::EResourceState::LXResourceState_Unloaded;
	Load();
	return true;
}

bool LXMaterial::Compile()
{
	if (State != EResourceState::LXResourceState_Loaded)
	{
		CHK(0);
		return false;
	}

	// Release the existing devices resources, the Renderer will recreate them.
	ReleaseDeviceMaterials();

	for (LXMaterialInstance* instance : Instances)
	{
		instance->Compile();
	}

	return true;
}

const LXPropertyAssetPtr* LXMaterial::GetPropertyTextureByName(const LXString& textureName) const
{
	return dynamic_cast<LXPropertyAssetPtr*>(GetProperty(textureName));
}

bool LXMaterial::GetFloatParameter(const LXString& textureName, float& outValue) const
{
	if (LXGraphMaterial* graphMaterial = GetGraph())
	{
		return graphMaterial->GetFloatParameter(textureName, outValue);
	}
	else
	{
		return false;
	}
}

LXGraphMaterial* LXMaterial::GetGraph() const
{
	if (!_isLoaded)
	{
		CHK_ONCE(0);
		
		return nullptr;
	}
	
	return GraphMaterial.get();
}

void LXMaterial::ReleaseGraph()
{
	GraphMaterial->Clear();
}

LXString LXMaterial::GetShaderBaseName() const
{
	return GetFilepath().GetFilenameNoExt();
}

void LXMaterial::OnPropertyChanged(LXProperty* pProperty)
{
	if (GetRenderer())
	{
		GetRenderer()->UpdateDeviceMaterials(this);
	}
}

