//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "LXFrustum.h"
#include "LXEngine.h"
#include "LXMatrix.h"
#include "LXTexture.h" // LEFT RIGHT ...

LXFrustum::LXFrustum(void)
{
}

LXFrustum::~LXFrustum(void)
{
}

void LXFrustum::Update( const LXMatrix& mvp)
{
	m_Frustum[(int)EViewFrustumPlane::Right].Set(mvp[3] - mvp[0], mvp[7] - mvp[4], mvp[11] - mvp[8],  mvp[15] - mvp[12]);
	m_Frustum[(int)EViewFrustumPlane::Left].Set(mvp[3] + mvp[0], mvp[7] + mvp[4], mvp[11] + mvp[8],  mvp[15] + mvp[12]);

	m_Frustum[(int)EViewFrustumPlane::top].Set(mvp[3] - mvp[1], mvp[7] - mvp[5], mvp[11] - mvp[9],  mvp[15] - mvp[13]);
	m_Frustum[(int)EViewFrustumPlane::Down].Set(mvp[3] + mvp[1], mvp[7] + mvp[5], mvp[11] + mvp[9],  mvp[15] + mvp[13]);

	m_Frustum[(int)EViewFrustumPlane::Back].Set(mvp[3] - mvp[2], mvp[7] - mvp[6], mvp[11] - mvp[10], mvp[15] - mvp[14]);
	m_Frustum[(int)EViewFrustumPlane::Front].Set(mvp[3] + mvp[2], mvp[7] + mvp[6], mvp[11] + mvp[10], mvp[15] + mvp[14]);

	for (int i = 0; i < 6; i++) 
		m_Frustum[i].Normalize();
}

void LXFrustum::Update( vec3f* p )
{
	m_Frustum[(int)EViewFrustumPlane::Left].Set(p[0], p[1], p[4]);
	m_Frustum[(int)EViewFrustumPlane::Right].Set(p[3], p[7], p[6]);

	m_Frustum[(int)EViewFrustumPlane::Down].Set(p[5], p[1], p[2]);
	m_Frustum[(int)EViewFrustumPlane::top].Set(p[3], p[0], p[4]);

	m_Frustum[(int)EViewFrustumPlane::Back].Set(p[4], p[5], p[7]);
	m_Frustum[(int)EViewFrustumPlane::Front].Set(p[1], p[0], p[3]);

	for (int i = 0; i < 6; i++) 
		m_Frustum[i].Normalize();
}

bool LXFrustum::IsBoxIn( float x, float y, float z, float x2, float y2, float z2 ) const
{
	 for (int i = 0; i < 6; i++)
	 {
		if (m_Frustum[i].Distance(vec3f(x, y, z)) > 0.f) 
			continue;

		if (m_Frustum[i].Distance(vec3f(x, y, z2)) > 0.f) 
			continue;

		if (m_Frustum[i].Distance(vec3f(x, y2, z)) > 0.f) 
			continue;

		if (m_Frustum[i].Distance(vec3f(x, y2, z2)) > 0.f) 
			continue;

		if (m_Frustum[i].Distance(vec3f(x2, y, z)) > 0.f) 
			continue;

		if (m_Frustum[i].Distance(vec3f(x2, y, z2)) > 0.f) 
			continue;

		if (m_Frustum[i].Distance(vec3f(x2, y2, z)) > 0.f) 
			continue;

		if (m_Frustum[i].Distance(vec3f(x2, y2, z2)) > 0.f) 
			continue;

		return false;
	}
	 
	return true;
}

bool LXFrustum::IsSphereIn( const vec3f& vPoint, float radius ) const
{
	for(int i = 0; i < 6; i++ )	
	{
		float fDistance = m_Frustum[i].m_vNormal->DotProduct(vPoint) + m_Frustum[i].m_fDistance;
		if( fDistance <= -radius )
			return false;
	}
	return true;
}

bool LXFrustum::IsSphereIn( const vec3f& vPoint, float radius, EFrustumTestResult& ftr ) const
{
	ftr = EFrustumTestResult::Inside;

	for(int i = 0; i < 6; i++ )	
	{
		float fDistance = m_Frustum[i].m_vNormal->DotProduct(vPoint) + m_Frustum[i].m_fDistance;
		
		if( fDistance <= -radius )
		{
			ftr = EFrustumTestResult::Outside;
			return false;
		}

		if( fDistance < radius )
			ftr = EFrustumTestResult::Cut;
	}
	return true;
}

bool  LXFrustum::IsPointIn ( float x, float y, float z ) const
{
	for(int i = 0; i < 6; i++ )	
	{
		float fDistance = m_Frustum[i].m_vNormal->x * x + m_Frustum[i].m_vNormal->y * y + m_Frustum[i].m_vNormal->z * z + m_Frustum[i].m_fDistance;
		if( fDistance < 0 )
			return false;
	}
	return true;
}

bool LXFrustum::IsTriangleIn ( const vec3f& v0, const vec3f& v1, const vec3f& v2 ) const
{
	if (IsPointIn(v0.x, v0.y, v0.z))
		return true;
	else if (IsPointIn(v1.x, v1.y, v1.z))
		return true;
	else
		return IsPointIn(v2.x, v2.y, v2.z);
}