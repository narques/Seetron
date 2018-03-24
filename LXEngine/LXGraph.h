//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

class LXTexture;
class LXMaterial;
class LXBitmap;


class LXGraphNode
{

public:

	virtual ~LXGraphNode() {};
	virtual void LinkTo(LXGraphNode*){};
};

class LXGraph
{

public:

	LXGraph();
	virtual ~LXGraph();

	void AddNode(LXGraphNode* Node) { Nodes.push_back(Node); }
	virtual void AddLink(LXGraphNode* Node0, LXGraphNode* Node1);

	list<LXGraphNode*> Nodes;

};

