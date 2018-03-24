//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXObject.h"
#include "LXMouseEventHandler.h"
#include "LXActor.h"

class LXQueryManager;

class LXCORE_API LXQuery : public LXMouseEventHandler
{

	friend class LXQueryManager;

public:

	LXQuery();
	LXQuery(LXQueryManager* pQueryManager);
	virtual ~LXQuery(void);

	//
	// Misc
	// 

	bool					IsActive( ) const { return _bIsActive; }
	virtual bool			MouseMoved( ) const { return false; }

	virtual void			SetPickedPoint( const vec3f& p ){ }
	void					SetConstraint( EConstraint e) { _constraint = e;  }

protected:

	// OnMouseMove
	virtual void			MouseMoveOnNormal( const vec3f& vNormal ) { };
	virtual void			MouseMoveOnPosition( const vec3f& vPosition ) { };
		
	LXQueryManager*			_QueryManager;
	bool					_bIsActive;
	EConstraint				_constraint = EConstraint::None;
};
