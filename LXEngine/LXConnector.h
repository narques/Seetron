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

class LXCORE_API LXConnector : public LXSmartObject
{

public:

	LXConnector(LXNode* owner);
	LXConnector(LXNode* owner, EConnectorRole role);
	LXConnector(LXNode* owner, const LXConnectorTemplate* connectorTemplate, EConnectorRole role);
	LXConnector(LXNode* owner, EConnectorRole role, EConnectorType type);
	virtual ~LXConnector();

	LXNode* GetOwner() const { return _owner; }

private:

	void DefineProperties();

public:

	EConnectorRole Role = EConnectorRole::Undefined;
	EConnectorType Type = EConnectorType::Undefined;
	
	list<LXConnection*> Connections;

private:

	LXNode* _owner;
};

class LXCORE_API LXConnectorTemplate : public LXSmartObject
{

public:

	LXConnectorTemplate(const LXString& name, EConnectorType type);
	virtual ~LXConnectorTemplate();
	EConnectorType Type;
	LXString Code;
};
