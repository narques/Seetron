//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "LXConnection.h"
#include "LXConnector.h"
#include "LXNode.h"

LXConnector::LXConnector(LXNode* owner, EConnectorRole role)
{
	SetName(L"[CoName]");
	_owner = owner;
	Role = role;
	DefineProperties();
}

LXConnector::LXConnector(LXNode* owner, const LXConnectorTemplate* connectorTemplate, EConnectorRole role)
{
	SetName(connectorTemplate->GetName());
	_owner = owner;
	Role = role;
	Type = connectorTemplate->Type;
	DefineProperties();
}

LXConnector::LXConnector(LXNode* owner)
{
	_owner = owner;
	DefineProperties();
}

LXConnector::LXConnector(LXNode* owner, EConnectorRole role, EConnectorType type)
{
	SetName(L"[CoName]");
	_owner = owner;
	Role = role;
	Type = type;
	DefineProperties();
}

void LXConnector::DefineProperties()
{
	// Force to create the UID.
	// Used as link in the LXConnection object
	// The generated value will be replaced by the serialized value if exist.
	GetUID(true);

	DefineProperty(L"Role", (uint*)&Role); // Enum
	DefineProperty(L"Type", (uint*)&Type); // Enum
}

LXConnector::~LXConnector()
{
	CHK(Connections.size() == 0);
}

LXConnectorTemplate::LXConnectorTemplate(const LXString& name, EConnectorType type):
	Type(type)
{
	SetName(name);
}

LXConnectorTemplate::~LXConnectorTemplate()
{
}

