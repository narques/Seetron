//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXRenderPass.h"

#define LX_DEDICATED_TRANSPARENCY_BUFFER 0

class LXRenderPassTransparency : public LXRenderPass
{

public:

	LXRenderPassTransparency(LXRenderer*);
	virtual ~LXRenderPassTransparency();
	void Resize(uint Width, uint Height) override;
	void Render(LXRenderCommandList* RenderCommandList) override;

private:

	void CreateBuffers(uint Width, uint Height);
	void DeleteBuffers();
	
public:

	list<LXRenderCluster*>* _ListRenderClusterTransparents = nullptr;

#if LX_DEDICATED_TRANSPARENCY_BUFFER

	LXTextureD3D11* TextureColor = nullptr;
	LXRenderTargetViewD3D11* RenderTargetColor = nullptr;

#endif

};

