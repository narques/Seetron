//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXCore/LXObject.h"
#include "LXVec3.h"
#include "LXPoint.h"

class LXActor;
class LXPrimitive;
class LXActorMesh;

class LXENGINE_API LXMouseEventHandler : public virtual LXObject
{

public:

	LXMouseEventHandler(void);
	virtual ~LXMouseEventHandler(void);
	
	virtual void OnLButtonDown		(uint64 nFlags, LXPoint pntWnd ){}
	virtual void OnLButtonUp		(uint64 nFlags, LXPoint pntWnd ){}
	virtual void OnLButtonDblClk	(uint64 nFlags, LXPoint pntWnd ){}
	
	virtual void OnMButtonDown		(uint64 nFlags, LXPoint pntWnd ){}
	virtual void OnMButtonUp		(uint64 nFlags, LXPoint pntWnd ){}

	virtual void OnRButtonDown		(uint64 nFlags, LXPoint pntWnd ){}
	virtual void OnRButtonUp		(uint64 nFlags, LXPoint pntWnd ){}

	virtual void OnMouseWheel		(uint16 nFlags, short zDelta, LXPoint pntWnd ){}
	virtual void OnMouseMove		(uint64 nFlags, LXPoint pntWnd ){}

	virtual void OnLButtonDown		(uint64 nFlags, LXPoint pntWnd, LXPrimitive* pDrawable ){}
	virtual void OnLButtonUp		(uint64 nFlags, LXPoint pntWnd, LXPrimitive* pDrawable ){}

	virtual void OnLButtonDown		(uint64 nFlags, LXPoint pntWnd, LXActor* pDrawable ){}
	virtual void OnLButtonUp		(uint64 nFlags, LXPoint pntWnd, LXActor* pDrawable ){}

	virtual bool OnMouseMove		(uint64 nFlags, LXPoint pntWnd, const vec3f& vPoint ){ return false; }

	virtual void OnLButtonClick		( LXActor* pActor ){}
	virtual void OnLButtonClick		( LXPrimitive* pActor ){}


};
