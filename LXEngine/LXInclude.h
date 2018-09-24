//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXConfig.h"

//
// STL
//

#include <algorithm>
#include <list>
#include <map>
#include <memory>
#include <set>
#include <stack>
#include <unordered_map>
#include <vector>
using namespace std;

//
// Misc 
//

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
#include "LXString.h"
#include "LXVec2.h"
#include "LXVec3.h"
#include "LXVec4.h"
#include "LXMatrix.h"
#include "LXLogger.h"
#include "LXStatistic.h"
#include "LXThreadManager.h"

//
// Misc
//

#include <stdlib.h>
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

#ifdef LX_DEBUGFLAG
#define NA_DEBUG_CLIENTBLOCK new( _CLIENT_BLOCK, __FILE__, __LINE__)
#define _CRTDBG_MAP_ALLOC
#else
#define NA_DEBUG_CLIENTBLOCK
#endif