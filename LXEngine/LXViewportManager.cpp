//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "StdAfx.h"
#include "LXViewportManager.h"

#define FOR_EACH_VIEWPORT(code)																			\
	for (ListViewports::iterator It = _listViewports.begin(); It!= _listViewports.end(); It++)		\
	{																									\
		LXViewport* pViewport = *It;																	\
		code;																							\
	}																									\

LXViewportManager::LXViewportManager(void)
{
}

LXViewportManager::~LXViewportManager(void)
{
	CHK(_listViewports.size() == 0);
}

LXViewport* LXViewportManager::GetViewport( ) const
{
	if (_listViewports.size())
		return *_listViewports.begin();
	else
		return NULL;
}
