//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXQuery.h"
#include "LXWorldTransformation.h"

class LXActor;

class LXENGINE_API LXQueryTransform : public LXQuery
{
public:
	
	LXQueryTransform(void);
	virtual ~LXQueryTransform(void);

	// Overridden from LXQuery
	void OnLButtonDown( uint64 nFlags, LXPoint pntWnd, LXActor* Actor ) override;
	void OnMouseMove( uint64 nFlags, LXPoint pntWnd ) override;
	void OnLButtonUp( uint64 nFlags, LXPoint pntWnd ) override;
	void SetPickedPoint(const vec3f& p) override { _PickedPoint = p; }
	bool MouseMoved( ) const { return _IsMouseMove; }
	
private:

	ListActors				_listActors;
	vec3f					_PickedPoint; 
	LXWorldTransformation	_PickedWT;
	vec2f					_StartPosition;
	vec2f					_EndPosition;
	bool					_IsMouseMove;
};
