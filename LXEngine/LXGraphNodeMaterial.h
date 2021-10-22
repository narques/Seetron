//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------------
// LXReference
// 
// Provides various Material GraphNodes.
// Useful to extend nodes with properties (instead of implementing the UserProperty support in the GraphTemplate).
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXNode.h"
#include "LXTextureSampler.h"

class LXGraph;
class LXNodeTemplate;

class LXGraphNodeTextureSampler : public LXNode
{
public: 
	
	LXGraphNodeTextureSampler(LXGraph* graph);
	LXGraphNodeTextureSampler(LXGraph* graph, const LXNodeTemplate* nodeTemplate);
	virtual ~LXGraphNodeTextureSampler();

	const LXTextureSampler& GetTextureSampler() const { return _textureSampler; }

private:
	
	void DefineProperties();

private:

	LXTextureSampler _textureSampler;
};

