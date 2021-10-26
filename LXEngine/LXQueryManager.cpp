//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "pch.h"
#include "LXQueryManager.h"
#include "LXQueryTransform.h"
#include "LXProject.h"

LXQueryManager::LXQueryManager(const LXProject* pDocument):
LXManager(pDocument)
{
}

LXQueryManager::~LXQueryManager(void)
{
	LX_SAFE_DELETE(_pCurrentQuery);
}

//
// Mouse Events
//

void LXQueryManager::OnLButtonClick(LXActor* pActor)
{
	if (_pCurrentQuery)
		_pCurrentQuery->OnLButtonClick(pActor);
}

void LXQueryManager::OnLButtonClick(LXPrimitive* pDrawable)
{
	if (_pCurrentQuery)
		_pCurrentQuery->OnLButtonClick(pDrawable);
}

void LXQueryManager::OnLButtonDown(uint64 nFlags, LXPoint pntWnd, LXPrimitive* pDrawable)
{
	if (_pCurrentQuery)
		_pCurrentQuery->OnLButtonDown( nFlags, pntWnd, pDrawable);
}

void LXQueryManager::OnLButtonUp( uint64 nFlags, LXPoint pntWnd, LXPrimitive* pDrawable)
{
	if (_pCurrentQuery)
		_pCurrentQuery->OnLButtonUp(nFlags, pntWnd);
}

void LXQueryManager::OnLButtonUp( uint64 nFlags, LXPoint pntWnd)
{
	if (_pCurrentQuery)
		_pCurrentQuery->OnLButtonUp(nFlags, pntWnd);
}

bool LXQueryManager::OnMouseMove ( uint64 nFlags, LXPoint pntWnd, const vec3f& vPoint)
{
	CHK(0);
	return false;
}

void LXQueryManager::OnMouseMove( uint64 nFlags, LXPoint pntWnd )
{
	if (_pCurrentQuery)
		_pCurrentQuery->OnMouseMove(nFlags, pntWnd);
}

void LXQueryManager::PushQuery(LXQuery* pQuery)
{
	CHK(pQuery);
	if (!pQuery)
		return;

	_pCurrentQuery = pQuery;
}

void LXQueryManager::MouseMoveOnNormal( const vec3f& vNormal )
{
	if (_pCurrentQuery)
		_pCurrentQuery->MouseMoveOnNormal(vNormal);
}

void LXQueryManager::MouseMoveOnPostion( const vec3f& vPosition )
{
	if (_pCurrentQuery)
		_pCurrentQuery->MouseMoveOnPosition(vPosition);
}
