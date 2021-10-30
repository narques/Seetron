//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXCore/LXObject.h"

class LXPrimitive;

class LXLODSystem : public LXObject
{

public:

	void AddLOD(std::shared_ptr<LXPrimitive>);

private:

	std::vector<std::shared_ptr<LXPrimitive>> _LODData;
};

