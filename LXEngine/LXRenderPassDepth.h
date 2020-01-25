//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXRenderPass.h"

class LXRenderPassDepth : public LXRenderPass
{

public:

	LXRenderPassDepth(LXRenderer* renderer);
	void Render(LXRenderCommandList* rcl) override;

	// Object list
	list<LXRenderCluster*>* RenderClusters = nullptr;
};

