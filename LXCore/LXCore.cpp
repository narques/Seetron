// LXCore.cpp : Defines the exported functions for the DLL.
//

#include "pch.h"
#include "framework.h"
#include "LXCore.h"


// This is an example of an exported variable
LXCORE_API int nLXCore=0;

// This is an example of an exported function.
LXCORE_API int fnLXCore(void)
{
    return 0;
}

// This is the constructor of a class that has been exported.
CLXCore::CLXCore()
{
    return;
}
