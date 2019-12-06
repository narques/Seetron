//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXMacros.h"
#include "LXFlags.h"

enum class ERenderClusterRole
{
	Default = LX_BIT(0),
	PrimitiveBBox = LX_BIT(1),
	ActorBBox = LX_BIT(2),
	All = Default | PrimitiveBBox | ActorBBox
};

typedef LXFlags<ERenderClusterRole> LXFlagsRenderClusterRole;
