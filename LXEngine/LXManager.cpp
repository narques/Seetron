//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "pch.h"
#include "LXEngine.h"
#include "LXManager.h"
#include "LXMaterial.h"
#include "LXSettings.h"
#include "LXBitmap.h"
#include "LXMSXMLNode.h"

LXManager::LXManager (const LXProject* pDocument):
LXDocumentOwner(pDocument)
{
}

LXManager::~LXManager ()
{
}
