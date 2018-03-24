//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "LXMaterialNode.h"
#include "LXColor.h"
#include "LXTexture.h"
#include "LXMaterial.h"
#include "LXMemory.h" // --- Must be the last included ---

LXMaterialNode::LXMaterialNode(LXMaterial* InMaterial, EMaterialNodeType InType):Material(InMaterial),Type(InType)
{
	// Force to create the UID.
	// Used as link in the MaterialInputs
	// The generated value will be replaced by the serialized value if exist.
	GetUID(true); 

	// --------------------------------------------------------------------------------------------------------------
	LXProperty::SetCurrentGroup(L"Parameters");
	// --------------------------------------------------------------------------------------------------------------
}

LXMaterialNode::~LXMaterialNode()
{
}

void LXMaterialNode::OnPropertyChanged(LXProperty* pProperty)
{
	__super::OnPropertyChanged(pProperty);
	Material->OnPropertyChanged(pProperty);
}

//------------------------------------------------------------------------------------------------------

LXMaterialNodeFloat::LXMaterialNodeFloat(LXMaterial* InMaterial, const float& InValue) : LXMaterialNode(InMaterial, EMaterialNodeType::MaterialInputTypeFloat), Value(InValue)
{
	SetName("Float");
	LXPropertyFloat* PropertyFloat = DefineProperty(kMaterrialNode_ValuePropertyName, &Value);
	PropertyFloat->SetMinMax(0.f, 1.0f);
}

LXMaterialNodeFloat::~LXMaterialNodeFloat()
{
}

void LXMaterialNodeFloat::OnPropertyChanged(LXProperty* pProperty)
{
	__super::OnPropertyChanged(pProperty);

	if (pProperty->GetName() == kMaterrialNode_ValuePropertyName)
	{
//		Material->OnNodeFloatValueChanged(GetName(), Value);
	}
	else
	{
		CHK(0);
	}
}

//------------------------------------------------------------------------------------------------------

LXMaterialNodeColor::LXMaterialNodeColor(LXMaterial* InMaterial, const LXColor4f& InColor): LXMaterialNode(InMaterial, EMaterialNodeType::MaterialInputTypeColor),Value(InColor)
{
	SetName("Float4");
	DefineProperty(kMaterrialNode_ValuePropertyName, &Value);
}

LXMaterialNodeColor::~LXMaterialNodeColor()
{
}

void LXMaterialNodeColor::OnPropertyChanged(LXProperty* pProperty)
{
	__super::OnPropertyChanged(pProperty);

	if (pProperty->GetName() == kMaterrialNode_ValuePropertyName)
	{
//		Material->OnNodeColorValueChanged(GetName(), Value);
	}
	else
	{
		CHK(0);
	}
}

//------------------------------------------------------------------------------------------------------

LXMaterialNodeTextureSampler::LXMaterialNodeTextureSampler(LXMaterial* InMaterial, LXTexture* InTexture, EShaderFlags InAffectedShaders, uint InSlot) :
	LXMaterialNode(InMaterial, EMaterialNodeType::MaterialInputTypeTextureSampler),
	Texture(InTexture), 
	AffectedShaders(InAffectedShaders),
	Slot(InSlot)
{
	SetName("TextureSampler");
	DefineProperty(kMaterialNode_TexturePropertyName, (LXAssetPtr*)&Texture);
	auto PropPropertyAffectedShaders = DefinePropertyEnum(L"AffectedShaders", GetAutomaticPropertyID(), (uint*)&AffectedShaders);
	PropPropertyAffectedShaders->SetBitmask(true);
	PropPropertyAffectedShaders->AddChoice("VertexShader", (uint)EShader::VertexShader);
	PropPropertyAffectedShaders->AddChoice("HullShader", (uint)EShader::HullShader);
	PropPropertyAffectedShaders->AddChoice("DomainShader", (uint)EShader::DomainShader);
	PropPropertyAffectedShaders->AddChoice("GeometryShader", (uint)EShader::GeometryShader);
	PropPropertyAffectedShaders->AddChoice("PixelShader", (uint)EShader::PixelShader);
	DefineProperty("Slot", &Slot);
}

LXMaterialNodeTextureSampler::~LXMaterialNodeTextureSampler()
{
}

