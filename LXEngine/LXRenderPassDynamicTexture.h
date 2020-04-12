//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXRenderPass.h"

class LXRenderData;

class LXRenderPassDynamicTexture : public LXRenderPass
{

public:

	LXRenderPassDynamicTexture(LXRenderer* InRenderer);
	virtual ~LXRenderPassDynamicTexture();
	void Render(LXRenderCommandList* RenderCommandList) override;
	void Resize(uint Width, uint Height) override;
	void AwakeActor(LXRenderData* renderData);

private:

	void Render(LXRenderCommandList* RCL, LXRenderCluster* renderCluster);

private:

	list<LXRenderData*> _onDemandRenderData;
};
