//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "pch.h"
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

LXNode* LXConnector::GetFirstConnectedNode(const LXString& nodeName) const
{
	for (const LXConnection* connection : Connections)
	{
		LXNode* node = connection->Source->_owner;

		if (node->GetName() == nodeName)
		{
			return node;
		}
		else
		{
			for (const LXConnector* connector : node->Inputs)
			{
				LXNode* node = connector->GetFirstConnectedNode(nodeName);

				if (node && node->GetName() == nodeName)
				{
					return node;
				}
			}
		}
	}

	return nullptr;
}

LXConnectorTemplate::LXConnectorTemplate(const LXString& name, EConnectorType type):
	Type(type)
{
	SetName(name);
}

LXConnectorTemplate::~LXConnectorTemplate()
{
}

