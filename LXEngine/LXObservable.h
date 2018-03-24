//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXObject.h"
#include "LXObserver.h"

class LXCORE_API LXObservable : public virtual LXObject
{

public:

	LXObservable(void);
	virtual ~LXObservable(void);

	void AddObserver		( LXObserver* pObserver ); 
	void RemoveObserver		( LXObserver* pObserver ); 
	void SetNotifyObservers	( bool bNotifyListerners ) { m_bNotifyListerners = bNotifyListerners; }

protected:

	ListObservers		m_listObservers;
	bool				m_bNotifyListerners;

};
