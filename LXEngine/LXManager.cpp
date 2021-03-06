//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "StdAfx.h"
#include "LXCore.h"
#include "LXManager.h"
#include "LXMaterial.h"
#include "LXSettings.h"
#include "LXBitmap.h"
#include "LXMSXMLNode.h"
#include "LXMemory.h" // --- Must be the last included ---

LXManager::LXManager (const LXProject* pDocument):
LXDocumentOwner(pDocument)
{
}

LXManager::~LXManager ()
{
}
