//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "StdAfx.h"
#include "LXQuery.h"
#include "LXQueryManager.h"
#include "LXMemory.h" // --- Must be the last included ---

LXQuery::LXQuery()
{
	_bIsActive = false;
}

LXQuery::LXQuery(LXQueryManager* pQueryManager)
{
	_QueryManager = pQueryManager;
	_bIsActive = false;
}

LXQuery::~LXQuery(void)
{
	CHK(!_bIsActive);
}




