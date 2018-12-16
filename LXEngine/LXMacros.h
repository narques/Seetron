//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

// Log Macros

#define LogI(section, message, ...) Log2(ELogType::LogType_Info, L#section, message, __VA_ARGS__);
#define LogW(section, message, ...) Log2(ELogType::LogType_Warning, L#section, message, __VA_ARGS__);
#define LogE(section, message, ...) Log2(ELogType::LogType_Error, L#section, message, __VA_ARGS__);
#define LogD(section, message, ...) Log2(ELogType::LogType_Debug, L#section, message, __VA_ARGS__);
#define Trace(section, message, ...) Output(ELogType::LogType_Debug, L#section, message, __VA_ARGS__);

// Assertion Macros

#define CHK(__expr) { if (!(__expr)) { LXDebug::Assert(#__expr, __FILE__, __LINE__); }} // CHECK
#define VRF(__expr) { if (!(__expr)) { LXDebug::Assert(#__expr, __FILE__, __LINE__); }} // VERIFY

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
#define LX_UNREF(p)				{ p->Deref(); if (p->GetRefCount() == 0) delete p; }

#define LX_BIT(x) (1<<(x))