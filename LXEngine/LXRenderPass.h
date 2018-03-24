//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXObject.h"
#include "LXFormats.h"

class LXTextureD3D11;
class LXRenderCommandList;
class LXProject;
class LXViewport;
class LXRenderCluster;
class LXShaderD3D11;
class LXDepthStencilViewD3D11;
class LXRenderTargetViewD3D11;
class LXRenderer;
class LXPrimitiveD3D11;

enum class ERenderPass
{
	Depth,
	Shadow,
	GBuffer,
	Last // To count and iterate
};

class LXRenderPass : public LXObject
{
	
public:

	LXRenderPass(LXRenderer* InRenderer);
	virtual ~LXRenderPass();

	virtual void Render(LXRenderCommandList* RenderCommandList) = 0;
	virtual bool IsValid() const { return true; }
	virtual void RebuildShaders() { };
	virtual void Resize(uint Width, uint Height)=0;
	virtual const LXTextureD3D11* GetOutputTexture() const { CHK(0); return nullptr; }
	virtual const bool GetOutputTextures(vector<const LXTextureD3D11*>& outTextures) { CHK(0); return false; }

protected:

	void AddToViewDebugger(const LXString& Name, const LXTextureD3D11* TextureD3D11, ETextureChannel TextureChannel);

protected:

	// Ref.
	LXRenderer* Renderer;

private:

	map<LXString, pair<const LXTextureD3D11*, ETextureChannel>> _DebugTextures;
};

