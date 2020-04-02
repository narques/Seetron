//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXSmartObject.h"
#include "LXDelegate.h"

class LXConnection;
class LXConnector;
class LXNode;

class LXCORE_API LXGraph : public LXSmartObject
{

public:

	LXGraph();
	virtual ~LXGraph();

	void Clear();

	LXConnection* CreateConnection(LXConnector* source, LXConnector* destination);
	virtual void AddNode(LXNode* node);
	virtual void RemoveNode(LXNode* node);
	void DeleteConnection(LXConnection* connection);
	void DeleteConnector(LXConnector* connector);


	const LXNode* GetMain() const;

protected:

	void OnLoaded() override;

public:

	LXDelegate<> OnGraphChanged;
	list<LXNode*> Nodes;
	list<LXConnection*> Connections;

private:

	LXNode * _main = nullptr;
};

