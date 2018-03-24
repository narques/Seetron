//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "LXRenderPass.h"
#include "LXRenderer.h"
#include "LXMemory.h"

LXRenderPass::LXRenderPass(LXRenderer* InRenderer):Renderer(InRenderer)
{
}

LXRenderPass::~LXRenderPass()
{
}

void LXRenderPass::AddToViewDebugger(const LXString& Name, const LXTextureD3D11* TextureD3D11, ETextureChannel TextureChannel)
{
	_DebugTextures[Name] = pair<const LXTextureD3D11*,ETextureChannel>(TextureD3D11, TextureChannel);
}

