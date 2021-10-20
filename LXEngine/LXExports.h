//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#ifdef LX_ENGINE_STATIC
#define LXENGINE_API
#else
#ifdef LXENGINE_EXPORTS
#define LXENGINE_API __declspec(dllexport)
#else
#define LXENGINE_API __declspec(dllimport)
#endif // LXENGINE_EXPORTS
#endif // LX_ENGINE_STATIC

