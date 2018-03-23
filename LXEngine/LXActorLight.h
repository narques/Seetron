//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXVec3.h"
#include "LXColor.h"
#include "LXActorMesh.h"
#include "LXCore.h"

class LXRenderer;
class LXActorCamera;

#define LX_MAX_INTENSITY 1000000.f

class LXCORE_API LXActorLight : public LXActorMesh
{

public:

	enum LXLightType
	{
		LX_LIGHT_SPOT,
		LX_LIGHT_DIRECTIONAL,
		LX_LIGHT_OMNIDIRECTIONAL
	};

private:
	
	LXActorLight(LXActorLight &);
	LXActorLight& operator=(LXActorLight&) = delete;

public:
		
	LXActorLight(LXProject* pDocument);
	virtual ~LXActorLight(void);

	// Overridden from LXSmartObject
	virtual void	OnTrashed() override;
	virtual void	OnRecycled() override;
	void			DefineProperties() ;
	virtual void	OnPropertyChanged(LXProperty* pProperty) override;

	// Overridden from LXActor
	virtual bool	ParticipateToSceneBBox( ) const override { return false; };
	virtual bool	IsVisible			( ) const override { return _bVisible && !GetCore().GetPlayMode(); }

#if LX_ANCHOR
	// Overridden from LXActor
	virtual void	OnAnchorMove		( LXAnchor* pAnchor, const LXMatrix& lcs) override;
	virtual	void	OnAnchorReleased	( LXAnchor* pAnchor ) override;
#endif

	// Properties
	void			SetDirection		( const vec3f& vDirection );
	vec3f			GetDirectionWCS		( ) ;
		
	void			UpdateTargetDistance( float fDistance );

private:

	GetSet(bool, _bLight, Light);
	GetSet(LXLightType, _eLightType, Type);
	GetSetDef(LXColor4f, _color, Color, WHITE);
	GetSetDef(float, _intensity, Intensity, 1.f);
	GetSetDef(bool, _bCastShadow, CastShadow, false);

	// Spot
	GetSetDef(float, _fSpotAngle, SpotAngle, 35.f);
	
	float _fTargetDistance = 1.f; 

};

typedef list<LXActorLight*> ListLights;

