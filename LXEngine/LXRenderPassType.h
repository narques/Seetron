//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

enum class ERenderPass
{
	Depth,
	Shadow,
	GBuffer,
	Transparency,
	RenderToTexture,
	Last // To count and iterate
};
