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
#include "LXGraph.h"
#include "LXMemory.h" // --- Must be the last included ---

LXGraph::LXGraph()
{
	// --------------------------------------------------------------------------------------------------------------
	LXProperty::SetCurrentGroup(L"Graph");
	// --------------------------------------------------------------------------------------------------------------
	DefineProperty("Nodes", (ArraySmartObjects*)&Nodes);
	DefineProperty("Connections", (ArraySmartObjects*)&Connections);
}

LXGraph::~LXGraph()
{
}

void LXGraph::AddNode(LXNode* node)
{
	Nodes.push_back(node);
}

void LXGraph::AddConnection(LXConnection* connection)
{
	Connections.push_back(connection);
}
