//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXObject.h"
#include "LXVec2.h"
#include "LXVec3.h"
#include "LXPoint.h"
#include "LXWorldTransformation.h"
#include "LXFlags.h"

enum LXMouseButton
{
	LX_LBUTTON = LX_BIT(0),
	LX_MBUTTON = LX_BIT(1),
	LX_RBUTTON = LX_BIT(2)
};

enum LXKeyState
{
	LX_NOKEY	= 0,
	LX_CONTROL	= LX_BIT(1),
	LX_SHIFT	= LX_BIT(2),
	LX_ALT		= LX_BIT(3)
};

class LXScene;
class LXProject;
class LXViewport;

class LXENGINE_API LXTrackBallCameraManipulator : public LXObject
{

public:

	LXTrackBallCameraManipulator(void);
	virtual ~LXTrackBallCameraManipulator(void);

	//
	// Device Inputs
	// 

	void OnButtonDown	( LXMouseButton eButton, int x, int y, bool Picked, const vec3f& POI);
	void OnButtonUp		( LXMouseButton eButton, int x, int y );
	bool OnMouseMove	( uint64 nFlags, int x, int y );
	void OnKeyDown		( LXKeyState eKey );
	void OnKeyUp		( LXKeyState eKey );
	void OnMouseWheel	( uint16 nFlags, short zDelta, LXPoint pntWnd );

	//
	// Misc
	// 

	const vec2f& GetPoint			( ) const { return _pntWnd; } 
	const vec2f& GetCurrentPoint	( ) const { return _pntCurrentWnd; }
	bool MouseHasMoved		( ) const { return _bMouseHasMoved; }
	bool IsManupated		( ) const { return _bManipulated; }
	bool ExtendedSelection	( ) const { return _bExtendedSelection; }
	void SetExtendedSelection (bool b) { _bExtendedSelection = b; }
	
	//
	// Animation 
	// 

	bool Update				( double dFrameTime );

private:

	bool RotateCamera		( double dFrameTime );
	bool PanCamera			( double dFrameTime );
	bool DollyCamera		( double dFrameTime );
	
private:

	//
	// Tools
	// 

	LXActorCamera*			GetCamera	( );
	LXScene*				GetScene	( );
	LXProject*				GetDocument	( );

public:

	LXViewport * _Viewport = nullptr;

private:

	bool					_bMouseHasMoved;      // Mouse has move between down and up
	bool					_bManipulated;
	bool					_bExtendedSelection;
	
	vec2f					_pntWnd;              // Window cursor position at mouse button down
	vec2f					_pntCurrentWnd;       // Window cursor position at mouse move

	float					_fPickedZ;	           // Window Z value of cursor position at mouse button down
	bool					_bPickedPoint;
	vec3f					_vPickedPoint;        // World cursor position at mouse button down
	LXWorldTransformation	_wtPicked;
		
	// Rotate
	vec2f					_pntWndRotate;

	// Pan
	bool					_bAnimatedPan = false;
	vec3f					_vPickedPointPan;
	float					_fPickedZPan;
	LXWorldTransformation	_wtPickedPan;

	// Zoom
	vec3f					_vPickedPointWhell;
	
	// Animation, Inertia

	vec2f					_rotateStart;
	vec2f					_rotateEnd;

	float					_dollyStart;
	float					_dollyEnd;

	vec2f					_panStart;
	vec2f					_panEnd;

	float					_distanceToMove = 0.f;
	int						_zDelta = 0;
};
