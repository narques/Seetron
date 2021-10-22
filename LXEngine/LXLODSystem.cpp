//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "LXLODSystem.h"

void LXLODSystem::AddLOD(std::shared_ptr<LXPrimitive> primitive)
{
	_LODData.push_back(primitive);
}
