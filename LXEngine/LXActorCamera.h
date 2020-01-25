//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXActor.h"
#include "LXVec3.h"
#include "LXMath.h"

class LXPrimitive;
class LXScene;
class LXViewState;

class LXCORE_API LXActorCamera : public LXActor
{

public:

	// Constructor, Destructor
	LXActorCamera(LXProject* pDocument);
	virtual ~LXActorCamera(void);

	// Overridden from LXActor
	virtual bool	ParticipateToSceneBBox( ) const override { return false; };

	// Misc
	void			Set				( float x, float y, float z, float tx, float ty, float tz);
	void			Set				( const vec3f& vPosition, const vec3f& vTarget);
	void			SetNearFar		( double dNear, double dFar ) { _fNear = (float)dNear; _fFar= (float)dFar; }
	
	void			LookAt			( double dAspect );
	void			MoveTo			( float fSpeed, vec3f& v);
	

	void			SetPosition		( const vec3f& vPosition ) { _bValidModelView = false; LXActor::SetPosition(vPosition);}
	
	
	vec3f			GetViewVector	( );
	vec3f*			GetTarget		( ) { return &_vTarget; }
	void			SetTarget		( const vec3f& vTarget );
	float			GetTargetDistance( ) const;
	
	void			SetFov			( float fFov ) { _fFov = fFov; }
	float			GetFov			( ) const { return _fFov; }

	void			SetHeight		( float fHeight	) { _fHeight = fHeight; }
	float			GetHeight		( ) const { return _fHeight; }
		
	float			GetAspect		( ) const { return _fAspect; }
	float			GetNear			( ) const { return _fNear; }
	float			GetFar			( ) const { return _fFar; }
	
	LXPrimitive*	GetDrawable		( ) const;

	float			GetFocal        ( ) const { return LX_LENSDIAG / (2.0f* tanf( _fFov / 2.0f ) ); }
	float			GetAperture		( ) const { return _fAperture; }
	
	LXMatrix&		GetMatrixView	( ) { CHK(_bValidModelView); return _modelView; }

	void			SetOrtho		( bool b ) { _bOrtho = b; }
	bool			IsOrtho			( ) const { return _bOrtho; }

private:

	void			DefineProperties( );
	void			LookAt			( );
	
private:

	bool			_bOrtho;

	vec3f			_vTarget;

	float			_fFov;
	float			_fHeight;
	float			_fAperture;
	float			_fAspect;
	float			_fNear;
	float			_fFar;

	LXMatrix		_modelView;  
	bool			_bValidModelView;
};
