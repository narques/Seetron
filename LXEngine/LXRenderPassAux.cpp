//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "LXActorCamera.h"
#include "LXProject.h"
#include "LXRenderPassAux.h"
#include "LXRenderTargetViewD3D11.h"
#include "LXRenderer.h"
#include "LXRenderClusterManager.h"
#include "LXRenderCommandList.h"
#include "LXViewport.h"
#include "LXFrustum.h"
#include "LXRenderPass.h"
#include "LXRenderTargetViewD3D11.h"
#include "LXRenderCluster.h"
#include "LXMemory.h" // --- Must be the last included ---

LXRenderPassAux::LXRenderPassAux(LXRenderer* InRenderer) :LXRenderPass(InRenderer)
{
}

LXRenderPassAux::~LXRenderPassAux()
{
}

void LXRenderPassAux::Render(LXRenderCommandList* RCL)
{
}

void LXRenderPassAux::Resize(uint Width, uint Height)
{
}
