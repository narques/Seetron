//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

// ---------------------------------------------------------------------------------
// --- Preprocessor definitions ---
// To add in the projects settings
// ---------------------------------------------------------------------------------

// Enable the static compilation 
// #define LX_CORE_STATIC: To build the player, samples, ...

// Enables the editor features: UI, Shader generator, ...
// #define LX_EDITOR

// Enable the dev features: Statistics, detailed logs, ...
// #define LX_DEV

// ---------------------------------------------------------------------------------
// --- File extensions ---
// ---------------------------------------------------------------------------------

#define LX_PROJECT_EXT				L"seproj"
#define LX_MATERIAL_EXT				L"smat"
#define LX_SHADER_EXT				L"hlsl"
#define LX_TEXTURE_EXT				L"stex"
#define LX_MESH_EXT					L"smesh"
#define LX_MESHBIN_EXT				L"sebin"
#define LX_PROCEDURALTEXTURE_EXT	L"sgtex"
#define LX_DEFAULT_EXT				L"xml"

// ---------------------------------------------------------------------------------
// --- Effects & Animations ---
// ---------------------------------------------------------------------------------

#define LX_ZOOMONBBOX_TIME 1000

// ---------------------------------------------------------------------------------
// --- Misc ---
// ---------------------------------------------------------------------------------

#define NOMINMAX


// 1 Uses win32 CreateMutex
// 0 Uses the std::mutex object
#define LX_WINDOWS_MUTEX 1

// ---------------------------------------------------------------------------------
// --- Third party ---
// ---------------------------------------------------------------------------------

//#define LX_ZIPARCHIRVE


// ---------------------------------------------------------------------------------
// --- Features ---
// ---------------------------------------------------------------------------------

#define LX_TRACE_OBJECTS 1						
#define LX_SUPPORT_LEGACY_FILE 1
#define LX_ANCHOR 0

// ---------------------------------------------------------------------------------
// --- Statistics ---
// ---------------------------------------------------------------------------------

#define LX_COUNTERS

// ---------------------------------------------------------------------------------
// --- Debug & Dev ---
// --------------------------------------------------------------------------------

#ifdef _DEBUG
#define LX_DEBUGFLAG     
//#define LX_VLD
#endif

// Add a "Visible" volatile property to LXPrimitive object.
#define LX_DEBUG_PRIMITIVE_PROPVISIBLE	 

