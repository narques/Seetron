//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXManager.h"
#include "LXProperty.h"
#include "LXCommand.h"
#include "LXCommandModifyProperty.h"
#include "LXAnimation.h"

class LXCORE_API LXAnimationManager : public LXSmartObject
{

public:

	LXAnimationManager(const LXProject* pDocument);
	virtual ~LXAnimationManager(void);

	// Overridden from LXSmartObject
	virtual bool		OnSaveChild(const TSaveContext& saveContext) override;
	virtual bool		OnLoadChild(const TLoadContext& loadContext) override;
	virtual void		GetChildren(ListSmartObjects&);
	
	// Animation
	void Play					( bool bPlay );
	bool Update					( double dFrameTime );
	void AddAnimation			( LXAnimation* pAnimation );
	LXAnimation* GetAnimation	( );

	// Key management
	void Remove(LXKey* pKey);
	void Add(LXKey* pKey);

	void PlayVolatileAnimation	( LXAnimation* pAnimation );

	// Scripts & Behaviors
	void AddBehavior			( LXActor* pGroup );
	void RemoveBehavior			( LXActor* pGroup );

private:

	LXAnimation*		_activeAnimation = NULL;
	ListAnimations		_listVolatileAnimations;
	ListAnimations		_listAnimations;

	SetActors			_setSceneObjectToUpdate;

	typedef map<LXActor*, LXMatrix> MapMatrices;
	MapMatrices			_mapMatrices;
};
