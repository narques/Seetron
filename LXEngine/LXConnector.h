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

enum class EConnectorRole
{
	Undefined,
	Input,
	Output
};

class LXCORE_API LXConnector : public LXSmartObject
{

public:

	LXConnector();
	LXConnector(EConnectorRole role);
	LXConnector(const LXConnectorTemplate* connectorTemplate, EConnectorRole role);
	virtual ~LXConnector();

private:

	void DefineProperties();

public:

	EConnectorRole Role = EConnectorRole::Undefined;
	EConnectorType Type = EConnectorType::Undefined;
	
	list<LXConnection*> Connections;
};

class LXCORE_API LXConnectorTemplate : public LXSmartObject
{

public:

	LXConnectorTemplate(const LXString& name, EConnectorType type);
	virtual ~LXConnectorTemplate();
	EConnectorType Type;
};
