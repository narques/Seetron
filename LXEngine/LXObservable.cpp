//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "StdAfx.h"
#include "LXObservable.h"
#include "LXCore.h"
#include "LXLogger.h"
#include "LXMemory.h" // --- Must be the last included ---

const LXString TAG = L"Observable";

LXObservable::LXObservable(void):
m_bNotifyListerners(true)
{
}

LXObservable::~LXObservable(void)
{
	CHK(!m_listObservers.size());
}

void LXObservable::AddObserver( LXObserver* pObserver ) 
{ 
	m_listObservers.push_back(pObserver); 

	LXString strObserver = pObserver->GetObjectName();
	LXString strObservable = this->GetObjectName();
	LogI(Observable, L"%s is observed by %s", strObservable.GetBuffer(), strObserver.GetBuffer());
}

void LXObservable::RemoveObserver( LXObserver* pObserver ) 
{ 
	m_listObservers.remove(pObserver); 
}