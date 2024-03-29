//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXDebug.h"

#include <windows.h>

// Assertion Macros

#define CHK(__expr) { \
	if (!(__expr)) \
	{ \
		if (IsDebuggerPresent()) \
			DebugBreak(); \
		else \
			LXDebug::LogFailure(#__expr, __FILE__, __LINE__); \
	} \
} \

#define CHK_ONCE(__expr) { \
	if (!(__expr)) \
	{ \
		static bool once = false; \
		if (!once) \
		{ \
			if (IsDebuggerPresent()) \
				DebugBreak(); \
			else \
				LXDebug::LogFailure(#__expr, __FILE__, __LINE__); \
		} \
		once = true;\
	} \
} \

#define VRF(__expr) CHK(__expr)

#define LX_CHK_RET(p)			{ CHK(p); if (!p) return; }
#define LX_CHK_RETF(p)			{ CHK(p); if (!p) return false; }
#define LX_CHK_RETV(p, ret)		{ CHK(p); if (!p) return ret; }

// Platform

#define LX_PRAGMA_OPTIM_OFF __pragma(optimize("",off))
#define LX_PRAGMA_OPTIM_ON __pragma(optimize("",on))
#define LX_INLINE __forceinline
#define LX_ALIGN(n) __declspec(align(n))

// Misc Macros

#define LX_SAFE_DELETE(p)       { if(p) { delete    (p);  p=nullptr; } }
#define LX_SAFE_RELEASE(p)      { if(p) { p->Release();	  p=nullptr; } }
#define LX_SAFE_DELETE_ARRAY(p) { if(p) { delete [] (p);  p=nullptr; } }

#define LX_BIT(x) (1<<(x))