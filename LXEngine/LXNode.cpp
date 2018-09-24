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

LXNode::LXNode(LXGraph* graph): Graph(graph)
{
	DefineProperties();
}

LXNode::LXNode(LXGraph* graph, const LXNodeTemplate* nodeTemplate): Graph(graph), NodeTemplate(nodeTemplate)
{
	SetName(nodeTemplate->GetName());
	TemplateID = nodeTemplate->GetName();
	Main = nodeTemplate->Main;

	for (const LXConnectorTemplate* connectorTemplate : nodeTemplate->Inputs)
	{
		Inputs.push_back(new LXConnector(this, connectorTemplate, EConnectorRole::Input));
	}

	for (const LXConnectorTemplate* connectorTemplate : nodeTemplate->Outputs)
	{
		Outputs.push_back(new LXConnector(this, connectorTemplate, EConnectorRole::Output));
	}

	DefineProperties();

	// Create the user properties according the templateType
	if (nodeTemplate->Type == "Variable")
	{
		EPropertyType type = GetPropertyTypeFromName(nodeTemplate->GetName());
		switch (type)
		{
		case EPropertyType::Float: CreateUserProperty<float>(L"Value", 0.0); break;
		case EPropertyType::Float2: CreateUserProperty<vec2f>(L"Value", vec2f(0.0, 0.0)); break;
		case EPropertyType::Float3: CreateUserProperty<vec3f>(L"Value", vec3f(0.0, 0.0, 0.0)); break;
		case EPropertyType::Float4: CreateUserProperty<vec4f>(L"Value", vec4f(0.0, 0.0, 0.0, 0.0)); break;
		case EPropertyType::AssetPtr: CreateUserProperty<LXAssetPtr>(L"Value", nullptr); break;
		default: CHK(0);
		}
	}
	else if (nodeTemplate->Type == "Constant")
	{
		EPropertyType type = GetPropertyTypeFromName(nodeTemplate->GetName());
		switch (type)
		{
		case EPropertyType::Float: CreateUserProperty<float>(L"Value", 0.0); break;
		case EPropertyType::Float2: CreateUserProperty<vec2f>(L"Value", vec2f(0.0, 0.0)); break;
		case EPropertyType::Float3: CreateUserProperty<vec3f>(L"Value", vec3f(0.0, 0.0, 0.0)); break;
		case EPropertyType::Float4: CreateUserProperty<vec4f>(L"Value", vec4f(0.0, 0.0, 0.0, 0.0)); break;
		default: CHK(0);
		}
	}
	
	if (nodeTemplate->GetName() == L"CustomFunction")
	{
		EditableInputs = true;
		CreateUserProperty<LXString>(L"Function", L"MyFunction");
		CreateUserProperty<LXAssetPtr>(L"Shader", nullptr);
	}
}

LXNode::~LXNode()
{
	for (LXConnector* connector : Inputs)
	{
		delete connector;
	}
 
	for (LXConnector* connector : Outputs)
	{
		delete connector;
	}
 
	Inputs.clear();
	Outputs.clear();
}

int LXNode::GetOuputConnectorIndex(const LXConnector* connector)
{
	int i = 0;
	for (const LXConnector* it : Outputs)
	{
		if (it == connector)
		{
			return i;
		}
		i++;
	}
	CHK(0);
	return -1;
}

void LXNode::DefineProperties()
{
	DefineProperty(L"TemplateID", &TemplateID);
	DefineProperty(L"Position", &Position);
	DefineProperty("Main", &Main);
	DefineProperty(L"Inputs", (ListSmartObjects*)&Inputs);
	DefineProperty(L"Outputs", (ListSmartObjects*)&Outputs);
}

void LXNode::OnLoaded()
{
// 	// Check if the 
// 	CHK(VerifyConformity(TemplateID))

	if (TemplateID == L"CustomFunction")
	{
		EditableInputs = true;
	}
}

void LXNode::GetNewListItemChoices(const LXPropertyListSmartObjects* property, list<LXString>& outStrings)
{
	outStrings.push_back(L"Float");
	outStrings.push_back(L"Float2");
	outStrings.push_back(L"Float3");
	outStrings.push_back(L"Float4");
	outStrings.push_back(L"Texture2D");
	outStrings.push_back(L"SamplerState");
}

void LXNode::AddItemToPropertyList(const LXPropertyListSmartObjects* property, const LXString& typeName)
{
	EConnectorType type =  GetConnectorTypeFromName(typeName);
	Inputs.push_back(new LXConnector(this, EConnectorRole::Input, type));
}


LXNodeTemplate::LXNodeTemplate()
{
	DefineProperty("Type", &Type);
	DefineProperty("Main", &Main);
	//DefineProperty(L"Inputs", (ListSmartObjects*)&Inputs); // GraphMaterial is not correctly formatted to do that... use OnLoadChild.
	//DefineProperty(L"Outputs", (ListSmartObjects*)&Outputs);
}

LXNodeTemplate::~LXNodeTemplate()
{
}

const LXConnectorTemplate* LXNodeTemplate::GetOutputConnectorTemplate(int index) const
{
	if (index < 0)
		return nullptr;
	return Outputs[index];
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
		LXString code = loadContext.node.attr(L"Code");
		EConnectorType type = GetConnectorTypeFromName(typeName.GetBuffer());
		LXConnectorTemplate* connectorTemplate = new LXConnectorTemplate(name, type);
		connectorTemplate->Code = code;
		Outputs.push_back(connectorTemplate);
		return true;
	}
	else if (loadContext.node.name() == L"Variable")
	{
		LXString name = loadContext.node.attr(L"Name");
		LXString declaration = loadContext.node.attr(L"Decl");
		LXVariableTemplate variable; { name, declaration; }
		//Variables.push_back(variable);
		Variables.push_back(LXVariableTemplate{name, declaration});
	}
	else
	{
		return false;
	}
	return true;
}
