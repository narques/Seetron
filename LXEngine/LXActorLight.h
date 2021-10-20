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
#include "LXEngine.h"

class LXRenderer;
class LXActorCamera;

#define LX_MAX_INTENSITY 1000000.f

enum class ELightType
{
	Spot,
	Directional,
	Omnidirectional
};

class LXENGINE_API LXActorLight : public LXActorMesh
{

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
	virtual bool	IsVisible			( ) const override { return _bVisible && !GetEngine().GetPlayMode(); }

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
	GetSet(ELightType, _eLightType, Type);
	GetSetDef(LXColor4f, _color, Color, WHITE);
	GetSetDef(float, _intensity, Intensity, 1.f);
	GetSetDef(bool, _bCastShadow, CastShadow, false);

	// Spot
	GetSetDef(float, _fSpotAngle, SpotAngle, 35.f);
	
	float _fTargetDistance = 1.f; 

};

typedef std::list<LXActorLight*> ListLights;

