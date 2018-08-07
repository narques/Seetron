//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "LXConnector.h"

LXConnector::LXConnector(EConnectorRole role)
{
	SetName(L"[CoName]");
	Role = role;
	DefineProperties();
}

LXConnector::LXConnector(const LXConnectorTemplate* connectorTemplate, EConnectorRole role)
{
	SetName(connectorTemplate->GetName());
	Role = role;
	DefineProperties();
}

LXConnector::LXConnector()
{
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
}

LXConnectorTemplate::LXConnectorTemplate(const LXString& name, EConnectorType type):
	Type(type)
{
	SetName(name);
}

LXConnectorTemplate::~LXConnectorTemplate()
{
}

