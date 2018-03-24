//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "LXGraph.h"
#include "LXMemory.h" // --- Must be the last included ---

LXGraph::LXGraph()
{
}

LXGraph::~LXGraph()
{
	for (LXGraphNode* GraphNode : Nodes)
	{
		delete GraphNode;
	}
}

void LXGraph::AddLink(LXGraphNode* Node0, LXGraphNode* Node1)
{
}
