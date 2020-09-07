//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "LXMaterialInstance.h"

//Seetron
#include "LXAsset.h"
#include "LXMaterial.h"
#include "LXPlatform.h"
#include "LXTexture.h"

LXMaterialInstance::LXMaterialInstance()
{
}

LXMaterialInstance::~LXMaterialInstance()
{
	Parent->Instances.remove(this);
}

shared_ptr<LXMaterialInstance> LXMaterialInstance::Create(const shared_ptr<LXMaterial>& material)
{
	shared_ptr<LXMaterialInstance> materialInstance = make_shared<LXMaterialInstance>();
	materialInstance->Parent = material;
	materialInstance->InstanceID = material->Instances.size();
	material->Instances.push_back(materialInstance.get());

	return materialInstance;
}

bool LXMaterialInstance::Load()
{
	return true;
}

bool LXMaterialInstance::Compile()
{
	// Parent material must be loaded, but no need to recreate its Device
	if (Parent->State == LXAsset::EResourceState::LXResourceState_Loaded)
	{
		// Release the existing instance device resources, the Renderer will recreate them.
		ReleaseDeviceMaterials();
		return true;
	}
	else
	{
		CHK(0);
		return false;
	}
}

LXString LXMaterialInstance::GetShaderBaseName() const
{
	return Parent->GetShaderBaseName() + L"_" + LXString::Number(InstanceID) + L"_";
}

bool LXMaterialInstance::IsTransparent() const
{
	return Parent->IsTransparent();
}

bool LXMaterialInstance::GetFloatParameter(const LXString& textureName, float& outValue) const
{
	return Parent->GetFloatParameter(textureName, outValue);
}

bool LXMaterialInstance::SetFloatParameter(const LXString& textureName, float value) const
{
	return Parent->SetFloatParameter(textureName, value);
}

EMaterialType LXMaterialInstance::GetType() const
{
	return Parent->GetType();
}

bool LXMaterialInstance::SetPropertyTextureByName(const LXString& textureName, const shared_ptr<LXTexture>& texture)
{
	const LXMaterial* material = Parent.get();

	LXProperty* property = material->GetProperty(textureName);

	if (const LXPropertyAssetPtr* propertyTexture = dynamic_cast<const LXPropertyAssetPtr*>(property))
	{

		LXPropertyAssetPtr* propertyOverride = dynamic_cast<LXPropertyAssetPtr*>(LXSmartObject::GetProperty(textureName));

		if (!propertyOverride)
		{
			propertyOverride = CreateUserProperty<LXAssetPtr>(textureName, propertyTexture->GetValue());
		}

		propertyOverride->SetValue(texture, false);
		return true;

	}

	return false;
}
