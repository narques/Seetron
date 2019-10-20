//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXObject.h"

class LXScene;
class LXActor;
class LXActorMesh;
class LXPrimitiveInstance;
class LXMatrix;
class LXWorldPrimitive;

class LXCORE_API LXTraverser : public LXObject
{

public:

	LXTraverser(void);
	virtual ~LXTraverser(void);

	void			SetScene( LXScene* pScene ) { _Scene = pScene; }
	virtual void	Apply( );
	
protected:

	virtual void	OnActor( LXActor* pActor );
	virtual void	OnMesh( LXActorMesh* pMesh );
	virtual void	OnPrimitive(LXActorMesh* pMesh, LXWorldPrimitive* WorldPrimitive){};
	
protected:

	LXScene*		_Scene = nullptr;
};
