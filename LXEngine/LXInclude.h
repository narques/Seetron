//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXConfig.h"

// LXCore
#include "LXCore/LXInclude.h"

//
// STD
//

#include <atomic>
#include <functional>
#include <memory>
#include <stack>
#include <unordered_map>

//
// Misc 
//

#include <assert.h>
#include <iosfwd>
#include <iostream>
#include <string>
#include <windows.h>
#include <wchar.h>

//
// Seetron
//

#include "LXExports.h"
#include "LXMacros.h"
#include "LXDebug.h"
#include "LXReference.h"
#include "LXTypes.h"
#include "LXAssetType.h"
#include "LXVec2.h"
#include "LXVec3.h"
#include "LXVec4.h"
#include "LXMatrix.h"

//
// Misc
//

#include <math.h>		// For sqrt
#include <float.h>		// For FLT_MAX

//
// Console
//

#include <io.h>
#include <fcntl.h>
#include <iostream>

//
// DirectX
//

#include <d2d1.h>

//
// Misc
//

#if !LX_WINDOWS_MUTEX
#include <mutex>
#endif
#include <omp.h>

//
// Memory Leak Detection
//

#if LX_DEBUGFLAG

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#define NA_DEBUG_CLIENTBLOCK new( _CLIENT_BLOCK, __FILE__, __LINE__)
#define new NA_DEBUG_CLIENTBLOCK

#else

#ifdef LX_VLD
#include <vld.h>
#endif

#endif


 
