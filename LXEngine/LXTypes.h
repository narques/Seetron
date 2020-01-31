//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include <vector>
#include <list>
#include <set>
using namespace std;

// 8-Bits
typedef char				int8;
typedef unsigned char       uint8;

// 16-Bits
typedef short				int16;
typedef unsigned short      uint16;

// 32-Bits
typedef int					int32;
typedef unsigned int		uint32;
typedef unsigned int		uint; 

// 64-Bits
typedef __int64				int64;
typedef unsigned __int64	uint64;

// Misc
typedef vector<int32>			ArrayInt;
typedef vector<uint32>			ArrayUint;

class LXSmartObject;
typedef list<LXSmartObject*>	ListSmartObjects;
typedef set<LXSmartObject*>		SetSmartObjects;
typedef vector<LXSmartObject*>	ArraySmartObjects;

class LXActor;
typedef list<LXActor*>			ListActors;
typedef set<LXActor*>			SetActors;
typedef vector<LXActor*>		ArrayActors;

class LXMaterial;
typedef set<LXMaterial*>		SetMaterials;

class LXKey;
typedef set<LXKey*>				SetKeys;
