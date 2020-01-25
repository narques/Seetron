//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXObject.h"

class LXTextureD3D11;
class LXRenderCommandList;
class LXProject;
class LXViewport;
class LXRenderCluster;
class LXShaderD3D11;
class LXDepthStencilViewD3D11;
class LXRenderTargetViewD3D11;
class LXPrimitiveD3D11;
class LXRenderer;

class LXRenderPass : public LXObject
{
	
public:

	LXRenderPass(LXRenderer* InRenderer);
	virtual ~LXRenderPass();

	virtual void Render(LXRenderCommandList* RenderCommandList) = 0;
	virtual bool IsValid() const { return true; }
	virtual void RebuildShaders() { };
	virtual void Resize(uint Width, uint Height) { };
	virtual const LXTextureD3D11* GetOutputTexture() const { CHK(0); return nullptr; }
	virtual const bool GetOutputTextures(vector<const LXTextureD3D11*>& outTextures) { CHK(0); return false; }
		
protected:

	// Ref.
	LXRenderer* Renderer;

private:

};

