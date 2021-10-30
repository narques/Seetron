//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXCore/LXObject.h"

class LXPrimitiveInstance;
typedef std::vector <std::shared_ptr<LXPrimitiveInstance>> VectorPrimitiveInstances;

class LXPrimitiveBase : LXObject
{
public:
};

class LXMeshData : public LXObject
{
public:
};


class LXMeshDataLegacy : public LXMeshData
{
	LXMeshDataLegacy();
	~LXMeshDataLegacy();
public:
	VectorPrimitiveInstances PrimitiveInstances;
};


class LXMeshDataMultiPrimitive : public LXMeshData
{
public:

	std::vector<LXPrimitiveInstance> PrimitiveInstances;
	
};