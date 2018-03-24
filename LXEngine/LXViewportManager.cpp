//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "StdAfx.h"
#include "LXViewportManager.h"
#include "LXCore.h"
#include "LXProject.h"
#include "LXScene.h"
#include "LXTraverserCallBack.h"
#include "LXPropertyManager.h"
#include "LXPrimitive.h"
#include "LXPrimitiveInstance.h"
#include "LXMemory.h" // --- Must be the last included ---

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

void LXViewportManager::OnChangeProperty ( const LXProperty* pProperty )
{
	if ((pProperty->GetID() == LXPropertyID::VISIBILITY) || 
		(pProperty->GetID() == LXPropertyID::POSITION) ||
		(pProperty->GetID() == LXPropertyID::ROTATION) || 
		(pProperty->GetID() == LXPropertyID::SCALE))
	{
		if (LXActor* pGroup = dynamic_cast<LXActor*>(pProperty->GetLXObject()))
		{

			if (!(pGroup->GetCID() & LX_NODETYPE_CAMERA))
				pGroup->InvalidateBounds(true);
		}
	}
}

void LXViewportManager::OnChangeHighlight( LXActor* pActor )
{
	FOR_EACH_VIEWPORT(pViewport->OnChangeHighlight(pActor));
}

LXViewport* LXViewportManager::GetViewport( ) const
{
	if (_listViewports.size())
		return *_listViewports.begin();
	else
		return NULL;
}

void LXViewportManager::ClearRenderList()
{
	for (LXViewport* pViewport : _listViewports)
		pViewport->ClearRenderList();
}
