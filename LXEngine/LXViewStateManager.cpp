//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "StdAfx.h"
#include "LXViewState.h"
#include "LXCore.h"
#include "LXCommandManager.h"
#include "LXViewStateManager.h"
#include "LXProject.h"
#include "LXScene.h"
#include "LXMSXMLNode.h"
#include "LXMemory.h" // --- Must be the last included ---

LXViewStateManager::LXViewStateManager(LXProject* pDocument):
_Project(pDocument)
{
}

LXViewStateManager::~LXViewStateManager(void)
{
	for (ListViewStates::iterator It = _listViewStates.begin(); It!= _listViewStates.end(); It++)
		delete *It;
}

bool LXViewStateManager::OnSaveChild(const TSaveContext& saveContext) const
{
	for (ListViewStates::const_iterator It = _listViewStates.begin(); It!= _listViewStates.end(); It++)
		(*It)->Save(saveContext);
	
	return true;
}

bool LXViewStateManager::OnLoadChild ( const TLoadContext& loadContext )
{
	if (loadContext.node.name() == L"LXViewState")
	{
		//if (loadContext.node.attr("UID") == LX_UID_VIEWSTATE_0)
		{
			if (_listViewStates.size() == 0)
			{
				LXViewState* pViewState = new LXViewState(_Project);
				pViewState->Load(loadContext);
				_listViewStates.push_back(pViewState);
			}
			else
			{
				CHK(0);
				return false;
			}
		}
		//else
		{
			//CHK(0); return false;
			//LXViewState* pViewState = new LXViewState(m_pDocument);
			//pViewState->Load(loadContext);
			//m_listViewStates.push_back(pViewState);
		}
	}
	else
		CHK(0);

	return true;
}

void LXViewStateManager::GetObjects( ListObjects& listObject )
{
	for (auto It = _listViewStates.begin(); It != _listViewStates.end(); It++)
	{
		listObject.push_back(*It);
	}
}

void LXViewStateManager::AddViewState(LXViewState* pViewState)
{
	CHK(pViewState);
	if (!pViewState)
		return;
	
	_listViewStates.push_back(pViewState);
}

LXViewState* LXViewStateManager::CreateViewState(LXProject* pDocument)
{
	CHK(pDocument);
	if (!pDocument)
		return NULL;

	LXViewState* pViewState = new LXViewState(pDocument);
	AddViewState(pViewState);
	return pViewState;
}

LXViewState* LXViewStateManager::GetViewState(const LXString& name)
{
	for (ListViewStates::iterator It = _listViewStates.begin(); It != _listViewStates.end(); It++)
	{
		if ((*It)->GetName() == name)
			return *It;
	}
	
	return NULL;
}