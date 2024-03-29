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

class LXConstantBufferD3D11;

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

	std::list<LXRenderCluster*>* _ListRenderClusterTransparents = nullptr;

#if LX_DEDICATED_TRANSPARENCY_BUFFER

	LXTextureD3D11* TextureColor = nullptr;
	LXRenderTargetViewD3D11* RenderTargetColor = nullptr;

#endif

	std::unique_ptr<LXConstantBufferD3D11> CBImageBaseLighting;
	
	struct
	{
		float Intensity;
		vec3f Pad;

	} CBImageBaseLightingData;

};

