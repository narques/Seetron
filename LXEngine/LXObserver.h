//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXObject.h"

class LXActor;
class LXSmartObject;
class LXProperty;

class LXCORE_API LXObserver : public virtual LXObject
{

public:

	LXObserver(void){};
	virtual ~LXObserver(void){};

};

typedef list<LXObserver*> ListObservers;

class LXCORE_API LXObserverCommandManager : public LXObserver
{

public:

	LXObserverCommandManager(void);
	virtual ~LXObserverCommandManager(void);

	virtual void OnFileSaved		( const LXSmartObject* pSmartObject ){};
	virtual void OnChangeProperty	( const LXProperty* pProperty ){};
	virtual void OnChangeSelection	( const SetSmartObjects& setSmartObjects ){};
	virtual void OnChangeHighlight	( LXActor* pActor ){};
	virtual void OnAddKeys			( const SetKeys& setKeys){};
	virtual void OnDeleteKeys		( const SetKeys& setKeys){};
	virtual void OnSceneChanged		( ){};
};
