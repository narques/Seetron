//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXSmartObject.h"

class LXConnector;
class LXNodeTemplate;
class LXConnectorTemplate;

enum class ENodeType
{
	Undefined,
	Variable,
	Function
};

class LXCORE_API LXNode : public LXSmartObject
{

public:

	LXNode();
	LXNode(LXNodeTemplate* nodeTemplate);
	virtual ~LXNode();

private:

	void DefineProperties();

public:

	LXString TemplateID;
	vec2f Position;
	LXNodeTemplate* NodeTemplate = nullptr;

	// Connector
	list<LXConnector*> Inputs;
	list<LXConnector*> Outputs;
};

class LXCORE_API LXNodeTemplate : public LXSmartObject
{

public:

	LXNodeTemplate();
	~LXNodeTemplate();

private:

	bool OnLoadChild(const TLoadContext& loadContext) override;

public:
		
	LXString Type;
	LXString Declaration;
			
	// Connector template
	list<LXConnectorTemplate*> Inputs;
	list<LXConnectorTemplate*> Outputs;

	
};