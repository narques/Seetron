//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#ifdef LXCORE_EXPORTS
#define LXCORE_API __declspec(dllexport)
#else
#define LXCORE_API __declspec(dllimport)
#endif