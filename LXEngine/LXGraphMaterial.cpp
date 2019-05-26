//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "LXGraphMaterial.h"
#include "LXConnector.h"
#include "LXMaterial.h"
#include "LXNode.h"
#include "LXTexture.h"

LXGraphMaterial::LXGraphMaterial(LXMaterial* material):Material(material)
{
	SetName(L"GraphMaterial");
}

LXGraphMaterial::~LXGraphMaterial()
{
}

void LXGraphMaterial::GetChildProperties(ListProperties& listProperties) const
{
	for (const LXNode* node : Nodes)
	{
		node->GetUserProperties(listProperties);
	}
}

LXTexture* LXGraphMaterial::GetTextureDisplacement(const LXString& textureName) const
{
	if (const LXNode* mainNode = GetMain())
	{
		if (const LXConnector* connector = mainNode->GetInputConnector(L"Displacement"))
		{
			if (const LXNode* node = connector->GetFirstConnectedNode(textureName))
			{
				LXProperty* property = node->GetProperty(L"Value");
				if (property && property->GetType() == EPropertyType::AssetPtr)
				{
					LXPropertyAssetPtr* propertyAssetPtr = (LXPropertyAssetPtr*)property;
					if (LXAsset* asset = propertyAssetPtr->GetValue())
					{
						return dynamic_cast<LXTexture*>(asset);
					}
				}
			}
		}
	}

	return nullptr;
}

bool LXGraphMaterial::GetFloatParameter(const LXString& paramName, float& outValue) const
{

	if (const LXNode* mainNode = GetMain())
	{
		if (const LXConnector* connector = mainNode->GetInputConnector(L"Displacement"))
		{
			if (const LXNode* node = connector->GetFirstConnectedNode(paramName))
			{
				LXProperty* property = node->GetProperty(L"Value");

				if (!property)
				{
					property = node->GetProperty(paramName);
				}

				if (property && property->GetType() == EPropertyType::Float)
				{
					LXPropertyFloat* propertyFloat = (LXPropertyFloat*)property;
					outValue = propertyFloat->GetValue();
					return true;
				}
			}
		}
	}
	
	return false;
}

void LXGraphMaterial::OnPropertyChanged(LXProperty* property)
{
	Material->OnPropertyChanged(property);
}
