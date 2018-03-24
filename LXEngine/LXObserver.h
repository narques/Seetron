//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
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

//------------------------------------------------------------------------

class LXCORE_API LXObserverCommandManager : public LXObserver
{

public:

	LXObserverCommandManager(void);
	virtual ~LXObserverCommandManager(void);

	virtual void OnFileSaved		( const LXSmartObject* pSmartObject ){};
	virtual void OnChangeProperty	( const LXProperty* pProperty ){};
	virtual void OnAddActors		( const SetActors& setActors ){};
	virtual void OnDeleteActors		( const SetActors& setActors ){};
	virtual void OnShowActors		( const SetActors& setActors ){};
	virtual void OnChangeSelection	( const SetSmartObjects& setSmartObjects ){};
	virtual void OnChangeHighlight	( LXActor* pActor ){};
	virtual void OnDeleteMaterials	( const SetMaterials& setMaterial ){};
	virtual void OnAddKeys			( const SetKeys& setKeys){};
	virtual void OnDeleteKeys		( const SetKeys& setKeys){};
	virtual void OnSceneChanged		( ){};
};

//------------------------------------------------------------------------

class LXObserverScene : public LXObserver
{

public:

	virtual void OnAddActors(const SetActors& setActors) {};
	virtual void OnDeleteActors(const SetActors& setActors) {};
	virtual void OnShowActors(const SetActors& setActors) {};
	virtual void OnHideActors(const SetActors& setActors) {};
};