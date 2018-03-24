//----------------------------------------------------------------------------------////------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#ifdef _DEBUG

#ifdef LX_DEBUGFLAG
	#define new NA_DEBUG_CLIENTBLOCK
#elif defined(LX_VLD)
	#include <vld.h>
#endif

#endif

