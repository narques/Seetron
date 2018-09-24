//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "StdAfx.h"
#include "LXAssetManager.h"
#include "LXController.h"
#include "LXGraphMaterial.h"
#include "LXGraphTemplate.h"
#include "LXMaterial.h"
#include "LXMaterialNode.h"
#include "LXNode.h"
#include "LXProject.h"
#include "LXMemory.h" // --- Must be the last included ---*

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

	// --------------------------------------------------------------------------------------------------------------
	LXProperty::SetCurrentGroup(L"PrecompiledData");
	// --------------------------------------------------------------------------------------------------------------

// 	// ConstantBuffer
// 	LXConstantBuffer ConstantBufferPS;
// 	DefineProperty("ConstantBufferPS", (LXAssetPtr*)&ConstantBufferPS);
// 
// 	// Texture lists
// 	list<LXAsset*> ListTexturesVS;
// 	DefineProperty("TexturesPS", (ListSmartObjects*)&ListTexturesVS);
// 
// 	list<LXAsset*> ListTexturesPS;
// 	DefineProperty("TexturesPS", (ListSmartObjects*)&ListTexturesPS);
// 	
// 	// Shader list
// 	list<LXAsset*> ListShaders;
// 	DefineProperty("Shaders", (ListSmartObjects*)&ListShaders);
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
		State = EResourceState::LXResourceState_Loaded;

	return false;
}

LXGraphMaterial* LXMaterial::GetGraph() const
{
	// Create the "Main" function if needed
	const LXNode* mainNode = GraphMaterial->GetMain();
	if (mainNode == nullptr)
	{
		const LXNodeTemplate* nodeTemplate = GetAssetManager()->GetGraphMaterialTemplate()->GetNodeTemplate(L"RenderTarget");
		CHK(nodeTemplate && nodeTemplate->Main);
		mainNode = new LXNode(GraphMaterial.get(), nodeTemplate);
		GraphMaterial->AddNode(const_cast<LXNode*>(mainNode));
	}
	
	return GraphMaterial.get();
}

void LXMaterial::ReleaseGraph()
{
	GraphMaterial->Clear();
}