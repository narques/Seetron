//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "LXNode.h"
#include "LXConnector.h"
#include "LXMSXMLNode.h"
#include "LXMemory.h" // --- Must be the last included ---

LXNode::LXNode()
{
	DefineProperties();
}

LXNode::LXNode(LXNodeTemplate* nodeTemplate):NodeTemplate(nodeTemplate)
{
	SetName(nodeTemplate->GetName());
	TemplateID = nodeTemplate->GetName();

	// Create the user properties according the templateType
	if (nodeTemplate->Type == "Variable")
	{
		EPropertyType type = GetPropertyTypeFromName(nodeTemplate->GetName());
		switch (type)
		{
		case EPropertyType::Float: CreateUserProperty<float>(L"Value", 0.0); break;
		case EPropertyType::Float2: CreateUserProperty<vec2f>(L"Value", vec2f(0.0, 0.0)); break;
		case EPropertyType::Float3: CreateUserProperty<vec3f>(L"Value", vec3f(0.0, 0.0, 0.0)); break;
		case EPropertyType::Float4: CreateUserProperty<vec4f>(L"Value", vec4f(0.0, 0.0,0.0,0.0)); break;
		default: CHK(0);
		}

	}
	
	for (const LXConnectorTemplate* connectorTemplate : nodeTemplate->Inputs)
	{
		Inputs.push_back(new LXConnector(connectorTemplate, EConnectorRole::Input));
	}

	for (const LXConnectorTemplate* connectorTemplate : nodeTemplate->Outputs)
	{
		Outputs.push_back(new LXConnector(connectorTemplate, EConnectorRole::Output));
	}

	DefineProperties();
}

LXNode::~LXNode()
{
}

void LXNode::DefineProperties()
{
	DefineProperty(L"TemplateID", &TemplateID);
	DefineProperty(L"Position", &Position);
	DefineProperty(L"Inputs", (ListSmartObjects*)&Inputs);
	DefineProperty(L"Outputs", (ListSmartObjects*)&Outputs);
}

// void LXNode::OnLoaded()
// {
// 	// Check if the 
// 	CHK(VerifyConformity(TemplateID))
// }

LXNodeTemplate::LXNodeTemplate()
{
	DefineProperty("Type", &Type);
	DefineProperty("Code", &Declaration);
	//DefineProperty(L"Inputs", (ListSmartObjects*)&Inputs); // GraphMaterial is not correctly formatted to do that... use OnLoadChild.
	//DefineProperty(L"Outputs", (ListSmartObjects*)&Outputs);
}

LXNodeTemplate::~LXNodeTemplate()
{
}

bool LXNodeTemplate::OnLoadChild(const TLoadContext& loadContext)
{
	if (loadContext.node.name() == L"Input")
	{
		LXString name = loadContext.node.attr(L"Name");
		LXString typeName = loadContext.node.attr(L"Type");
		EConnectorType type = GetConnectorTypeFromName(typeName.GetBuffer());
		LXConnectorTemplate* connectorTemplate = new LXConnectorTemplate(name, type);
		Inputs.push_back(connectorTemplate);
		return true;
	}
	else if (loadContext.node.name() == L"Output")
	{
		LXString name = loadContext.node.attr(L"Name");
		LXString typeName = loadContext.node.attr(L"Type");
		EConnectorType type = GetConnectorTypeFromName(typeName.GetBuffer());
		LXConnectorTemplate* connectorTemplate = new LXConnectorTemplate(name, type);
		Outputs.push_back(connectorTemplate);
		return true;
	}
	else
	{
		return false;
	}
	return true;
}
