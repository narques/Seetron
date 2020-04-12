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

void LXGraphMaterial::AddNode(LXNode* node)
{
	if (LXPropertyAssetPtr* property = dynamic_cast<LXPropertyAssetPtr*>(node->GetProperty(L"Value")))
	{
		_assetTextures.insert(pair<LXString, LXPropertyAssetPtr*>(node->GetName(), property));
	}
	
	__super::AddNode(node);
}

void LXGraphMaterial::RemoveNode(LXNode* node)
{
	if (LXPropertyAssetPtr* property = dynamic_cast<LXPropertyAssetPtr*>(node->GetProperty(L"Value")))
	{
		auto it = _assetTextures.begin();
		while (it != _assetTextures.end())
		{
			if (it->first == node->GetName() && it->second == property)
			{
				it = _assetTextures.erase(it);
			}
			else
			{
				it++;
			}
		}
	}

	__super::RemoveNode(node);
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
					if (LXAsset* asset = propertyAssetPtr->GetValue().get())
					{
						return dynamic_cast<LXTexture*>(asset);
					}
				}
			}
		}
	}

	return nullptr;
}

LXPropertyAssetPtr* LXGraphMaterial::GetPropertyTextureByName(const LXString& textureName) const
{
	auto it = _assetTextures.find(textureName);
	if (it != _assetTextures.end())
		return it->second;
	else
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

void LXGraphMaterial::OnLoaded()
{
	_assetTextures.clear();

	for (LXNode* node : Nodes)
	{
		if (LXPropertyAssetPtr* property = dynamic_cast<LXPropertyAssetPtr*>(node->GetProperty(L"Value")))
		{
			_assetTextures.insert(pair<LXString, LXPropertyAssetPtr*>(node->GetName(), property));
		}

	}

	__super::OnLoaded();
}

void LXGraphMaterial::OnPropertyChanged(LXProperty* property)
{
	Material->OnPropertyChanged(property);
}
