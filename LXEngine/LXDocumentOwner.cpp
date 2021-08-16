//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "LXDocumentOwner.h"
#include "LXProject.h"
#include "LXQueryManager.h"

LXDocumentOwner::LXDocumentOwner():
_pDocument(NULL)
{
}

LXDocumentOwner::LXDocumentOwner(const LXProject* pDocument):
_pDocument((LXProject*)pDocument)
{
}

LXDocumentOwner::~LXDocumentOwner(void)
{
}

LXQueryManager* LXDocumentOwner::GetQueryManager( ) const
{
	if (!_pDocument)
		return NULL;

	return &_pDocument->GetQueryManager();
}

LXQuery* LXDocumentOwner::GetCurrentQuery( ) const 
{
	if (!_pDocument)
		return NULL;

	return _pDocument->GetQueryManager().GetCurrentQuery();
}

LXSelectionManager* LXDocumentOwner::GetSelectionManager( ) const
{
	if (!_pDocument)
		return NULL;

	return &_pDocument->GetSelectionManager();
}