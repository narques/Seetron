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
#include "LXMaterial.h"
#include "LXMaterialNode.h"
#include "LXProject.h"
#include "LXMemory.h" // --- Must be the last included ---*

LXMaterial::LXMaterial(EMaterialType InMaterialType):MaterialType(InMaterialType)
{
	SetName(L"Material");
	DefineProperties();
}

LXMaterial::~LXMaterial(void)
{
	for (LXMaterialNode* MaterialNode : MaterialNodes)
	{
		delete MaterialNode;
	}
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
	
	// --------------------------------------------------------------------------------------------------------------
	LXProperty::SetCurrentGroup(L"Shaders");
	// --------------------------------------------------------------------------------------------------------------
	
	DefineProperty("VertexShader", (LXAsset**)&VertexShader);
	DefineProperty("HullShader", (LXAsset**)&HullShader);
	DefineProperty("DomainShader", (LXAsset**)&DomainShader);
	DefineProperty("GeometryShader", (LXAsset**)&GeometryShader);
	DefineProperty("PixelShader", (LXAsset**)&PixelShader);
	
	// --------------------------------------------------------------------------------------------------------------
	LXProperty::SetCurrentGroup(L"Nodes");
	// --------------------------------------------------------------------------------------------------------------

	DefineProperty("Nodes", (ArraySmartObjects*)&MaterialNodes);

	// --------------------------------------------------------------------------------------------------------------
	LXProperty::SetCurrentGroup(L"Outputs");
	// --------------------------------------------------------------------------------------------------------------
	
	DefineProperty("Color", &Color);		
	DefineProperty("Opacity", &Opacity);
	DefineProperty("Normal", &Normal);
	DefineProperty("Metallic", &Metallic);	
	DefineProperty("Roughness", &Roughness);
	DefineProperty("Emissive", &Emissive);
	DefineProperty("Displacement", &Displacement);
}

void LXMaterial::GetChildProperties(ListProperties& listProperties) const
{
	__super::GetChildProperties(listProperties);

	if (Version == 1)
	{
		for (LXMaterialNode* MaterialNode : MaterialNodes)
		{
			if (LXProperty* Property = MaterialNode->GetProperty(kMaterrialNode_ValuePropertyName))
			{
				Property->SetLabel(MaterialNode->GetName());
				listProperties.push_back(Property);
			}
			else if (LXProperty* Property = MaterialNode->GetProperty(kMaterialNode_TexturePropertyName))
			{
				Property->SetLabel(MaterialNode->GetName());
				listProperties.push_back(Property);
			}
		}
	}
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

void LXMaterial::SetColor(const LXColor4f& InColor)
{
	Color = AddColor(InColor);
}

void LXMaterial::SetColor(const LXString& Filename, uint Slot)
{
	if (LXMaterialNode* MaterialNode = AddTexture(Filename, EShader::PixelShader, Slot))
	{
		Color = MaterialNode;
		MaterialNode->Link = EMaterialLink::LinkToColor;
	}
}

void LXMaterial::GetTextures(list<LXTexture*>& listTextures)
{
	for (LXMaterialNode* MaterialNode : MaterialNodes)
	{
		if (LXMaterialNodeTextureSampler* MIT = CastMaterialNode<LXMaterialNodeTextureSampler>(MaterialNode))
		{
			LXTexture* Texture = MIT->GetTexture();
			CHK(Texture);
			listTextures.push_back(Texture);
		}
	}
}

void LXMaterial::GetTextureSamplers(list<LXMaterialNodeTextureSampler*>& listTextureSamplers) const 
{
	for (LXMaterialNode* MaterialNode : MaterialNodes)
	{
		if (LXMaterialNodeTextureSampler* MIT = CastMaterialNode<LXMaterialNodeTextureSampler>(MaterialNode))
			listTextureSamplers.push_back(MIT);
	}
}

LXMaterialNode* LXMaterial::GetMaterialNodeFromUID(const LXString& UID)
{
	for (LXMaterialNode* MaterialNode : MaterialNodes)
	{
		CHK(MaterialNode->GetUID() != nullptr);
		if (*MaterialNode->GetUID() == UID)
			return MaterialNode;
	}
	return nullptr;
}

void LXMaterial::SetVertexShader(const LXString& Filename)
{
	LXAssetManager& rm = GetCore().GetProject()->GetAssetManager();
	LXShader* Shader = rm.GetShader(Filename);
	VertexShader = Shader;
}

void LXMaterial::SetPixelShader(const LXString& Filename)
{
	LXAssetManager& rm = GetCore().GetProject()->GetAssetManager();
	LXShader* Shader = rm.GetShader(Filename);
	PixelShader = Shader;
}

void LXMaterial::AddMaterialNode(LXMaterialNode* MaterialNode)
{
	MaterialNodes.push_back(MaterialNode);
	GetController()->AddMaterialToUpdateRenderStateSet(this);
}

LXMaterialNode* LXMaterial::AddTexture(const LXString& strFilepath,EShaderFlags Shaders, uint Slot)
{
	LXAssetManager& rm = GetCore().GetProject()->GetAssetManager();
	LXTexture* Texture = rm.GetTexture(strFilepath);
	LXMaterialNodeTextureSampler* MNTS = new LXMaterialNodeTextureSampler(this, Texture, Shaders, Slot);
	AddMaterialNode(MNTS);
	return MNTS;
}

LXMaterialNode* LXMaterial::AddTexture()
{
	return AddTexture(L"", EShader::PixelShader, 0);
}

LXMaterialNode* LXMaterial::AddFloat()
{
	LXMaterialNodeFloat* MNC = new LXMaterialNodeFloat(this, 0.0f);
	AddMaterialNode(MNC);
	return MNC;
}

LXMaterialNode* LXMaterial::AddColor(const LXColor4f& InColor)
{
	LXMaterialNodeColor* MNC = new LXMaterialNodeColor(this, InColor);
	AddMaterialNode(MNC);
	return MNC;
}

LXMaterialNode* LXMaterial::AddColor()
{
	return AddColor(LXColor4f(0.f, 0.f, 0.f, 0.f));
}

LXMaterialNode* LXMaterial::GetMaterialNodeFromName(const LXString& Name)
{
	for (LXMaterialNode* MaterialNode : MaterialNodes)
	{
		if (MaterialNode->GetName() == Name)
		{
			return MaterialNode;
		}
	}
	
	return nullptr;
}

LXMaterialNodeFloat* LXMaterial::GetMaterialNodeFloatFromName(const LXString& Name)
{
	return dynamic_cast<LXMaterialNodeFloat*>(GetMaterialNodeFromName(Name));
}

LXMaterialNodeTextureSampler* LXMaterial::GetMaterialNodeTextureSamplerFromName(const LXString& Name)
{
	return dynamic_cast<LXMaterialNodeTextureSampler*>(GetMaterialNodeFromName(Name));
}
