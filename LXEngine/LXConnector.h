//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXSmartObject.h"

class LXConnection;
class LXConnectorTemplate;
class LXNode;

enum class EConnectorRole
{
	Undefined,
	Input,
	Output
};

class LXENGINE_API LXConnector : public LXSmartObject
{

public:

	LXConnector(LXNode* owner);
	LXConnector(LXNode* owner, EConnectorRole role);
	LXConnector(LXNode* owner, const LXConnectorTemplate* connectorTemplate, EConnectorRole role);
	LXConnector(LXNode* owner, EConnectorRole role, EConnectorType type);
	virtual ~LXConnector();

	LXNode* GetOwner() const { return _owner; }
	LXNode* GetFirstConnectedNode(const LXString& nodeName) const;

private:

	void DefineProperties();

public:

	EConnectorRole Role = EConnectorRole::Undefined;
	EConnectorType Type = EConnectorType::Undefined;
	
	std::list<LXConnection*> Connections;

private:

	LXNode* _owner;
};

class LXENGINE_API LXConnectorTemplate : public LXSmartObject
{

public:

	LXConnectorTemplate(const LXString& name, EConnectorType type);
	virtual ~LXConnectorTemplate();
	EConnectorType Type;
	LXString Code;
};
