//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

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
typedef std::vector<int32>			ArrayInt;
typedef std::vector<uint32>			ArrayUint;

class LXSmartObject;
typedef std::list<LXSmartObject*>	ListSmartObjects;
typedef std::set<LXSmartObject*>		SetSmartObjects;
typedef std::vector<LXSmartObject*>	ArraySmartObjects;

class LXActor;
typedef std::list<LXActor*>			ListActors;
typedef std::set<LXActor*>			SetActors;
typedef std::vector<LXActor*>		ArrayActors;

class LXMaterial;
typedef std::set<LXMaterial*>		SetMaterials;

class LXKey;
typedef std::set<LXKey*>				SetKeys;
