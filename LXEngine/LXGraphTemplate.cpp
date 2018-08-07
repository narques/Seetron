//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "LXGraphTemplate.h"
#include "LXMemory.h" // --- Must be the last included --- 

LXGraphTemplate::LXGraphTemplate()
{
	DefineProperty("Nodes", (ArraySmartObjects*)&NodeTemplates);
}

LXGraphTemplate::~LXGraphTemplate()
{
}

bool LXGraphTemplate::OnLoadChild(const TLoadContext& loadContext)
{
	return true;
}
