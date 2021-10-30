//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXManager.h"
#include "LXCore/LXObject.h"
#include "LXQuery.h"

class LXProject;
class LXGizmo;
class LXViewport;

class LXENGINE_API LXQueryManager : public LXMouseEventHandler, public LXManager
{

  friend class LXQuery;
  
public:

	LXQueryManager(const LXProject* pDocument);
	virtual ~LXQueryManager(void);

	void					PushQuery( LXQuery* pQuery );

	LXQuery*				GetCurrentQuery( ){ return _pCurrentQuery; }
	
	void					SetDocument( LXProject* pDoc ) { _pDocument = pDoc; }
	LXProject*				GetDocument( ) { return _pDocument; }
	LXViewport*				GetViewport( ) const { return _pViewport; }
	void					SetViewport( LXViewport* pViewport ) { _pViewport = pViewport; }
	
	// Overridden from LXMouseEventHandler
	virtual void			OnLButtonClick	( LXActor* pActor ) override;
	virtual	void			OnLButtonClick	( LXPrimitive* pDrawable)  override;
	virtual void			OnLButtonDown	( uint64 nFlags, LXPoint pntWnd, LXPrimitive* pDrawable ) override;
	virtual void			OnLButtonUp		( uint64 nFlags, LXPoint pntWnd, LXPrimitive* pDrawable ) override;
	virtual bool			OnMouseMove		( uint64 nFlags, LXPoint pntWnd, const vec3f& vPoint ) override;
	void					OnMouseMove		( uint64 nFlags, LXPoint pntWnd );
	void					OnLButtonUp		( uint64 nFlags, LXPoint pntWnd);

	void					MouseMoveOnNormal( const vec3f& vNormal );
	void					MouseMoveOnPostion( const vec3f& vPosition );
	
private:

	LXQuery*				_pCurrentQuery = nullptr;
	LXProject*				_pDocument = nullptr;
	LXViewport*				_pViewport = nullptr;
};

