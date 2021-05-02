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

	// Animation
	void Play					( bool bPlay );
	bool Update					( double dFrameTime );
	void SetAnimation			( LXAnimation* pAnimation );
	LXAnimation* GetAnimation	( );

	// Key management
	void Remove(LXKey* pKey);
	void Add(LXKey* pKey);

	void PlayVolatileAnimation	( LXAnimation* pAnimation );

	// Scripts & Behaviors
	void AddBehavior			( LXActor* pGroup );
	void RemoveBehavior			( LXActor* pGroup );

private:

	LXAnimation*		_animation = nullptr;
	LXAnimation*		_activeAnimation = nullptr;
	ListAnimations		_listVolatileAnimations;
	
	SetActors			_setSceneObjectToUpdate;

	typedef std::map<LXActor*, LXMatrix> MapMatrices;
	MapMatrices			_mapMatrices;
};
