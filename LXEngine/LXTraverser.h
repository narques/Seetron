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
class LXActorMesh;
class LXComponentMesh;
class LXScene;
class LXWorldPrimitive;

class LXENGINE_API LXTraverser : public LXObject
{

public:

	LXTraverser();
	virtual ~LXTraverser();

	void			SetScene( LXScene* pScene ) { _Scene = pScene; }
	virtual void	Apply( );
	
protected:

	virtual void	OnActor( LXActor* pActor );
	virtual void	OnMesh( LXActorMesh* pMesh );
	virtual void	OnMesh( LXComponentMesh* pMesh );
	virtual void	OnPrimitive(LXActorMesh* actorMesh, LXComponentMesh* componentMesh, LXWorldPrimitive* WorldPrimitive){};
	
protected:

	LXScene*		_Scene = nullptr;
};
