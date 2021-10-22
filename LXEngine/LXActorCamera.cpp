//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "StdAfx.h"
#include "LXEngine.h"
#include "LXActorCamera.h"
#include "LXProject.h"
#include "LXPropertyManager.h"
#include "LXScene.h"
#include "LXViewState.h"

LXActorCamera::LXActorCamera(LXProject* pDocument):
LXActor(pDocument),
_bOrtho(false),
_bValidModelView(false)
{
	SetName(L"Camera");
	_nCID |= LX_NODETYPE_CAMERA;
	
	_Transformation.SetTranslation(vec3f(-0.25f, -1.0f, 0.25));
	_vTarget.Set(0.0f, 0.0f, 0.0f);
	_fFov = 55.0f;	
	_fHeight = 1000.f;
	_fAperture = 2.8f;
	_fAspect = 1.0f;
	_fNear = 10.0f;		 // 10cm
	_fFar = KM2CM(10.f); // 10km 
	
	DefineProperties();
}

LXActorCamera::~LXActorCamera(void)
{
}

void LXActorCamera::DefineProperties ( ) 
{
	// Target
	LXPropertyVec3f* pPropTarget = DefinePropertyVec3f( L"Target", LXPropertyID::CAMERA_TARGET, &_vTarget);
	pPropTarget->SetAnimatable(true);
		
	// Fov
	LXPropertyFloat* pPropFloat = DefinePropertyFloat( L"Fov", LXPropertyID::CAMERA_FOV, &_fFov);
	
	// Ortho
	DefinePropertyBool( L"CameraOrtho", LXPropertyID::CAMERA_ORTHO, &_bOrtho);
	
	// Height (Ortho)
	DefinePropertyFloat( L"CameraHeight", LXPropertyID::CAMERA_HEIGHT, &_fHeight);
	
	// Aperture
	DefinePropertyFloat( L"Aperture", LXPropertyID::CAMERA_APERTURE, &_fAperture);
}

void LXActorCamera::LookAt(double dAspect)
{
	_fAspect = (float)dAspect;
	LookAt();
}

void LXActorCamera::LookAt(void)
{
	_modelView.SetIdentity();

	LXMatrix matCamera;

	vec3f vz = GetViewVector();

	// No _Project when used in RenderPassShadow
	vec3f vy = _Project?_Project->GetUpAxis():vec3f(0.f,0.f,1.0f);
	vec3f vx = CrossProduct(vy, vz);
	vx.Normalize();
	if (!IsValid(vx))
	{
		vy = vec3f(0.f, 1.f, 0.0f);
		vx = CrossProduct(vy, vz);
		vx.Normalize();
	}

	vy.CrossProduct(vz, vx);
	vy.Normalize();

	matCamera.SetXYZ(vx, vy, vz);

	_modelView = Inverse(matCamera);
	
	vec3f vo = vec3f(_Transformation.GetTranslation());
	CHK(IsValid(vo));

	LXMatrix matPosition;
	matPosition.SetOrigin(-vo.x, -vo.y, -vo.z);

	_modelView = _modelView * matPosition;
	
	_bValidModelView = true;
}

void LXActorCamera::Set(float x, float y, float z, float tx, float ty, float tz)
{
	SetPosition(vec3f(x, y, z));
	_vTarget.Set(tx, ty, tz);
}

void  LXActorCamera::Set( const vec3f& vPosition, const vec3f& vTarget)
{
	CHK(IsValid(vPosition));
	CHK(IsValid(vTarget));
	SetPosition(vPosition);
	_vTarget = vTarget;
}

void LXActorCamera::MoveTo(float fDistance, vec3f& v)
{
	vec3f vPosition = GetPosition();
	vec3f vVector = v - vPosition;
	vVector.Normalize();

	vPosition.x += vVector.x * fDistance;		 
	vPosition.y += vVector.y * fDistance;		 
	vPosition.z += vVector.z * fDistance;	

	SetPosition(vPosition);

	_vTarget.x += vVector.x * fDistance;		  
	_vTarget.y += vVector.y * fDistance;		  
	_vTarget.z += vVector.z * fDistance;	
}

vec3f LXActorCamera::GetViewVector()
{
	vec3f v = GetMatrix().GetVx();
	v.Normalize();
	return v;
}

LXPrimitive* LXActorCamera::GetDrawable( ) const
{
	return NULL;
}

void LXActorCamera::SetTarget(const vec3f& vTarget)
{
	CHK(IsValid(vTarget));
	_vTarget = vTarget;
}

float LXActorCamera::GetTargetDistance() const
{
	return _vTarget.Distance(GetPosition());
}
