//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "LXNode.h"
#include "LXAssetManager.h"
#include "LXConnector.h"
#include "LXEngine.h"
#include "LXGraph.h"
#include "LXGraphTemplate.h"
#include "LXMSXMLNode.h"

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

ENodeType LXNode::GetType() const
{
	if (NodeTemplate->Type == L"Constant")
		return ENodeType::Constant;
	else if (NodeTemplate->Type == L"Variable")
		return ENodeType::Variable;
	else if (NodeTemplate->Type == L"Function")
		return ENodeType::Function;
	else
		return ENodeType::Undefined;
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
	LXProperty* property = DefineProperty(L"TemplateID", &TemplateID);
	property->SetReadOnly(true);
	property = DefineProperty(L"Position", &Position);
	property->SetReadOnly(true);
	property = DefineProperty("Main", &Main);
	property->SetReadOnly(true);
	property = DefineProperty(L"Inputs", (ListSmartObjects*)&Inputs);
	property->SetReadOnly(true);
	property = DefineProperty(L"Outputs", (ListSmartObjects*)&Outputs);
	property->SetReadOnly(true);
}

void LXNode::OnLoaded()
{
	const LXGraphTemplate* graphTemplate = GetAssetManager()->GetGraphMaterialTemplate();
	NodeTemplate = graphTemplate->GetNodeTemplate(TemplateID);

// 	// Check if the 
// 	CHK(VerifyConformity(TemplateID))
	Update(TemplateID);

	if (TemplateID == L"CustomFunction")
	{
		EditableInputs = true;
		LXPropertyListSmartObjects* property = dynamic_cast<LXPropertyListSmartObjects*>(GetProperty(L"Inputs"));
		property->SetReadOnly(false);
		property->GetChoiceNames.AttachLambda([](ArrayStrings& arrayStrings)
			{
				for (int i = 0; i < (int)EConnectorType::Last; i++)
				{
					arrayStrings.push_back(GetConnectorTypename((EConnectorType)i));
				}
			});

		property->OnAddItem.AttachMemberLambda([this](const LXString& name)
			{
				EConnectorType type = GetConnectorTypeFromName(name);
				LXConnector* connector = new LXConnector(this, EConnectorRole::Input, type);
				connector->SetName(name);
				this->Inputs.push_back(connector);
			});

		property->OnRemoveItem.AttachMemberLambda([this](LXSmartObject* item)
			{
				LXConnector* connector = dynamic_cast<LXConnector*>(item);
				std::find(this->Inputs.begin(), this->Inputs.end(), item);
				this->Inputs.remove(connector);
				Graph->DeleteConnector(connector);
			});
	}

	const LXNodeTemplate* nodeTemplate = GetAssetManager()->GetGraphMaterialTemplate()->GetNodeTemplate(TemplateID);
	
	if ((nodeTemplate->Type == "Variable") || (nodeTemplate->Type == "Constant"))
	{
		GetProperty(LXPropertyID::NAME)->SetReadOnly(false);
	}
	else
	{
		GetProperty(LXPropertyID::NAME)->SetReadOnly(true);
	}
}

void LXNode::OnPropertyChanged(LXProperty* property)
{
	Graph->OnPropertyChanged(property);
}

void LXNode::Update(const LXString& templateID)
{
	//
	// For now , update only the RenderTarget Connector,
	// as the connector "reference" between Node and NodeTemplate is based on the connector name,
	// so it is not possible to know which is new or simply renamed in the template.
	//

	if (TemplateID != L"RenderTarget")
	{
		return;
	}	

	const LXNodeTemplate* nodeTemplate = GetAssetManager()->GetGraphMaterialTemplate()->GetNodeTemplate(TemplateID);

	for (const LXConnectorTemplate* connectorTemplate : nodeTemplate->Inputs)
	{
		auto it = std::find_if(Inputs.begin(), Inputs.end(), [connectorTemplate](LXConnector* connector)
		{
			return connector->GetName() == connectorTemplate->GetName();
		});

		if (it == Inputs.end())
		{
			Inputs.push_back(new LXConnector(this, connectorTemplate, EConnectorRole::Input));
		}
	}

	for (const LXConnectorTemplate* connectorTemplate : nodeTemplate->Outputs)
	{
		auto it = std::find_if(Outputs.begin(), Outputs.end(), [connectorTemplate](LXConnector* connector)
		{
			return connector->GetName() == connectorTemplate->GetName();
		});
		if (it == Outputs.end())
		{
			Outputs.push_back(new LXConnector(this, connectorTemplate, EConnectorRole::Output));
		}
	}
}

void LXNode::GetNewListItemChoices(const LXPropertyListSmartObjects* property, std::list<LXString>& outStrings)
{
	outStrings.push_back(L"Float");
	outStrings.push_back(L"Float2");
	outStrings.push_back(L"Float3");
	outStrings.push_back(L"Float4");
	outStrings.push_back(L"Texture2D");
	outStrings.push_back(L"SamplerState");
}

LXSmartObject* LXNode::AddItemToPropertyList(const LXPropertyListSmartObjects* property, const LXString& typeName)
{
	EConnectorType type =  GetConnectorTypeFromName(typeName);
	LXConnector* connector = new LXConnector(this, EConnectorRole::Input, type);
	Inputs.push_back(connector);
	return connector;
}

const LXConnector* LXNode::GetInputConnector(const LXString& connectorName) const
{
	auto it = std::find_if(Inputs.begin(), Inputs.end(), [connectorName](LXConnector* connector)
	{
		return connector->GetName() == connectorName;
	});

	return it != Inputs.end() ? *it : nullptr;

}

const LXConnector* LXNode::GetOutputConnector(const LXString& connectorName) const
{
	auto it = std::find_if(Outputs.begin(), Outputs.end(), [connectorName](LXConnector* connector)
	{
		return connector->GetName() == connectorName;
	});

	return it != Inputs.end() ? *it : nullptr;
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
	for (LXConnectorTemplate* connectorTemplate : Inputs)
	{
		delete connectorTemplate;
	}

	for (LXConnectorTemplate* connectorTemplate : Outputs)
	{
		delete connectorTemplate;
	}
}

const LXConnectorTemplate* LXNodeTemplate::GetOutputConnectorTemplate(const LXConnector* connector) const
{
	auto it = std::find_if(Outputs.begin(), Outputs.end(), [connector](LXConnectorTemplate* connectorTemplate)
	{
		return connectorTemplate->GetName() == connector->GetName();
	});

	return it != Outputs.end() ? *it : nullptr;

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
