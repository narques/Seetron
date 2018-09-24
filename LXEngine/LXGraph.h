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
class LXNode;

class LXCORE_API LXGraph : public LXSmartObject
{

public:

	LXGraph();
	virtual ~LXGraph();

	void Clear();
	
	void AddNode(LXNode* node);
	void AddConnection(LXConnection* connection);

	void DeleteNode(LXNode* node);
	void DeleteConnection(LXConnection* conection);

	const LXNode* GetMain() const;

private:

	void OnLoaded() override;

public:

	list<LXNode*> Nodes;
	list<LXConnection*> Connections;

private:

	LXNode * _main = nullptr;
};

