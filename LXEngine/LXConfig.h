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
// #define LX_ENGINE_STATIC: To build the player, samples, ...

// Enables the editor features: UI, Shader generator, ...
// #define LX_EDITOR

// Enable the dev features: Statistics, detailed logs, ...
// #define LX_DEV

// ---------------------------------------------------------------------------------
// --- File extensions ---
// ---------------------------------------------------------------------------------

#define LX_PROJECT_EXT				L"seproj"
#define LX_MATERIAL_EXT				L"smat"
#define LX_MATERIALINSTANCE_EXT		L"smi"
#define LX_SHADER_EXT				L"hlsl"
#define LX_TEXTURE_EXT				L"stex"
#define LX_ANIMATION_EXT			L"anim"
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

#define LX_TRACE_OBJECTS 0						
#define LX_ANCHOR 0

// ---------------------------------------------------------------------------------
// --- Threads ---
// ---------------------------------------------------------------------------------

#define  LX_LOADING_THREAD 0

// ---------------------------------------------------------------------------------
// --- Statistics ---
// ---------------------------------------------------------------------------------

#define LX_COUNTERS

// ---------------------------------------------------------------------------------
// --- Rendering ---
// ---------------------------------------------------------------------------------

#define LX_MAX_LODS 8

// ---------------------------------------------------------------------------------
// --- Debug & Dev ---
// --------------------------------------------------------------------------------

#define LX_BUILD_SHADERS_FOR_ALL_PASSES 0

// Add the frame # to log entries when 'outputted'
// useful to set the log TimeSlicing budget.
#define LX_LOG_OUTPUT_FRAME 0

#ifdef _DEBUG
#define LX_DEBUGFLAG 0
//#define LX_VLD
#else
#define LX_DEBUGFLAG 0
#endif

// Add a "Visible" volatile property to LXPrimitive object.
#define LX_DEBUG_PRIMITIVE_PROPVISIBLE	 

