//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

class LXSpacePartitioning
{

public:

	LXSpacePartitioning();
	~LXSpacePartitioning();
	void BuildOctree();
	void BuildBSP();

	// Bounding Volume Hierarchy
	void BuildBVH();
};

LXSpacePartitioning* GetSpacePartitioning();


