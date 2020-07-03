//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "LXGraphMaterial.h"

// Seetron
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

void LXGraphMaterial::AddNode(shared_ptr<LXNode>& node)
{
	Register(node);
	__super::AddNode(node);
}

void LXGraphMaterial::RemoveNode(shared_ptr<LXNode>& node)
{
	Unregister(node);
	__super::RemoveNode(node);
}

void LXGraphMaterial::GetChildProperties(ListProperties& listProperties) const
{
	for (const shared_ptr<LXNode>& node : Nodes)
	{
		node->GetUserProperties(listProperties);
	}
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
	for (const shared_ptr<LXNode>& node : Nodes)
	{
		Register(node);
	}

	__super::OnLoaded();
}

void LXGraphMaterial::OnPropertyChanged(LXProperty* property)
{
	Material->OnPropertyChanged(property);
}

void LXGraphMaterial::Register(const shared_ptr<LXNode>& node)
{
	if (const LXProperty* property = node->GetProperty(L"Value"))
	{
		void* var = property->GetVarPtr();
		LXProperty* newProperty = property->Create(node->GetName(), var);
		newProperty->SetPersistent(false);
		Material->AddProperty(newProperty);
	}
}

void LXGraphMaterial::Unregister(const shared_ptr<LXNode>& node)
{
	if (const LXProperty* property = node->GetProperty(L"Value"))
	{
		LXProperty* propertyFromMaterial = Material->GetProperty(node->GetName());
		CHK(propertyFromMaterial);
		CHK(Material->RemoveProperty(propertyFromMaterial));
		delete propertyFromMaterial;
	}
}
