//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#ifdef LX_CORE_STATIC
#define LXCORE_API
#else
#ifdef LXCORE_EXPORTS
#define LXCORE_API __declspec(dllexport)
#else
#define LXCORE_API __declspec(dllimport)
#endif // LXCORE_EXPORTS
#endif // LX_CORE_STATIC

