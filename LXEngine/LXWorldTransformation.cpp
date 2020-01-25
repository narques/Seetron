//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "StdAfx.h"
#include "LXWorldTransformation.h"
#include "LXBBox.h"
#include "LXCamera.h"
#include "LXActorCamera.h"
#include "LXVec4.h"
#include "LXAxis.h"
#include "LXLogger.h"
#include "LXMemory.h" // --- Must be the last included ---

LXWorldTransformation::LXWorldTransformation(void):
m_bValid(false)
{
}

/*virtual*/
LXWorldTransformation::~LXWorldTransformation(void)
{
}

void LXWorldTransformation::UnProject(const double winx, const double winy, const double winz, double& upx, double& upy, double& upz) const 
{
	CHK(m_bValid);
	if (!m_bValid)
		return;
	
	vec4f vPoint;

	vPoint.x=((float)winx-(float)m_vp[0])/(float)m_vp[2]*2.0f-1.0f;
	vPoint.y=((float)winy-(float)m_vp[1])/(float)m_vp[3]*2.0f-1.0f;
	vPoint.z=2.0f*(float)winz-1.0f;
	vPoint.w=1.0f;

	m_matVPInv.LocalToParentPoint(vPoint);

	CHK(vPoint.w != 0.f);

	upx = vPoint.x / vPoint.w;
	upy = vPoint.y / vPoint.w;
	upz = vPoint.z / vPoint.w;

	//LogD(WorldTransformation, L"Unproject %.2f %.2f %.2f to %.2f %.2f %.2f", winx, winy, winz, upx, upx, upx);
}

void LXWorldTransformation::UnProject2WCS(const double winx, const double winy, const double winz, double& upx, double& upy, double& upz) const 
{
	CHK(m_bValid);
	if (!m_bValid)
		return;

	LXMatrix p = m_matProjection;
	p.Inverse2();

	vec4f vPoint;

	vPoint.x=((float)winx-(float)m_vp[0])/(float)m_vp[2]*2.0f-1.0f;
	vPoint.y=((float)winy-(float)m_vp[1])/(float)m_vp[3]*2.0f-1.0f;
	vPoint.z=2.0f*(float)winz-1.0f;
	vPoint.w=1.0f;

	p.LocalToParentPoint(vPoint);

	CHK(vPoint.w != 0.f);

	upx = vPoint.x / vPoint.w;
	upy = vPoint.y / vPoint.w;
	upz = vPoint.z / vPoint.w;
}

void LXWorldTransformation::UnProject( vec3f& v )
{
	double dx, dy, dz;
	UnProject((double)v.x, (double)v.y, (double)v.z, dx, dy, dz);
	v.Set((float)dx, (float)dy, (float)dz);
}

void LXWorldTransformation::UnProject2WCS( vec3f& v )
{
	double dx, dy, dz;
	UnProject2WCS((double)v.x, (double)v.y, (double)v.z, dx, dy, dz);
	v.Set((float)dx, (float)dy, (float)dz);
}

void LXWorldTransformation::Project( const double x, const double y, const double z, double& winx, double& winy, double& winz )
{
	CHK(m_bValid);
	if (!m_bValid)
		return;

	vec4f vPoint((float)x, (float)y, (float)z, 1.0f);
	m_matVP.LocalToParentPoint(vPoint);

	//CHK(vPoint.w != 0.f);
	// TODO: IsNull()...
	if (vPoint.w == 0.f)
		return;
		
	//[-1 +1] 
	vPoint.x /= vPoint.w;
	vPoint.y /= vPoint.w;
	vPoint.z /= vPoint.w;

	// Not possible to test as the points beyond the NAR/FAR give out of range z
// 	CHK((vPoint.x > -1) && (vPoint.x < 1)); 
// 	CHK((vPoint.y > -1) && (vPoint.y < 1));
// 	CHK((vPoint.z > -1) && (vPoint.z < 1));
	
	winx = (vPoint.x * 0.5f + 0.5f) * m_vp[2] + m_vp[0];
	winy = (vPoint.y * 0.5f + 0.5f) * m_vp[3] + m_vp[1];
	winz = (1.0f + vPoint.z) * 0.5f;

	//LogD(WorldTransformation, L"Project %.2f %.2f %.2f to %.2f %.2f %.2f", x, y, z, winx, winy, winz);
}

void LXWorldTransformation::Project( LXBBox* pBox )
{
	#if 0 //def LX_USE_VEC3M128
	vec3M128 vMin = const_cast<vec3M128&>(pBox->GetMin());
	vec3M128 vMax = const_cast<vec3M128&>(pBox->GetMax());
	#else
	vec3f vMin = const_cast<vec3f&>(pBox->GetMin());
	vec3f vMax = const_cast<vec3f&>(pBox->GetMax());
	#endif
	Project(vMin);
	Project(vMax);
	pBox->Reset();
	pBox->Add(vMin);
	pBox->Add(vMax);
}

void LXWorldTransformation::Project(vec3f& v)
{
	double winx, winy, winz;
	Project(v.x, v.y, v.z, winx, winy, winz);
	v.Set((float)winx, (float)winy, (float)winz);
}

void LXWorldTransformation::CreateOrtho( int nWidth, int nHeight, float l, float r, float b, float t, float n, float f )
{
	m_vp[0] = 0;
	m_vp[1] = 0;
	m_vp[2] = (float)nWidth;
	m_vp[3] = (float)nHeight;
	m_matProjection.SetOrtho( l, r, b, t, n, f );
	m_matProjectionInv = m_matProjection;
	m_matProjectionInv.Inverse();

	m_matView.SetIdentity();
	//m_matMVP = m_matProjection.MultProj(m_matModelView); //TODO : MultProjOrtho
	m_matViewInv = m_matView;
	m_matViewInv.Inverse();
	m_bValid = true;
}

void LXWorldTransformation::CreateOrtho( int nWidth, int nHeight )
{
	m_vp[0] = 0;
	m_vp[1] = 0;
	m_vp[2] = (float)nWidth;
	m_vp[3] = (float)nHeight;
	m_matProjection.SetOrtho( 0.0, (float)nWidth, (float)nHeight, 0.0, 0.0, 1.0 );
	m_matProjectionInv = m_matProjection;
	m_matProjectionInv.Inverse();
	
	m_matView.SetIdentity();
	//m_matMVP = m_matProjection.MultProj(m_matModelView); //TODO : MultProjOrtho
	m_matViewInv = m_matView;
	m_matViewInv.Inverse();
	m_bValid = true;
}

void LXWorldTransformation::FromCamera(LXActorCamera* pCamera, int nWidth, int nHeight)
{
	m_vp[0] = 0;
	m_vp[1] = 0;
	m_vp[2] = (float)nWidth;
	m_vp[3] = (float)nHeight;

	if (pCamera->IsOrtho())
	{
		float fHeight = pCamera->GetHeight();
		float fWidth = fHeight * nWidth / nHeight;
		m_matProjection.SetOrtho( -fWidth, fWidth, -fHeight, fHeight, pCamera->GetNear(), pCamera->GetFar());
	}
	else
		m_matProjection.SetPerspectiveLH(pCamera->GetFov(), pCamera->GetAspect(), pCamera->GetNear(), pCamera->GetFar());
		//m_matProjection.BuildInfinitePerspectiveLH(pCamera->GetFov(), pCamera->GetAspect(), pCamera->GetNear());
		
	m_matProjectionInv = m_matProjection;
	m_matProjectionInv.Inverse2();

	// Redundancy LXWordTransformation vs LXCamera 
	m_matView = pCamera->GetMatrixView(); 
	
	m_matVP = m_matProjection * m_matView;
	m_matVPInv = m_matVP;
	m_matVPInv.Inverse2();

	m_matViewInv = m_matView;
	m_matViewInv.Inverse2();

	m_bValid = true;
} 

void LXWorldTransformation::FromCamera(LXCamera* camera, int nWidth, int nHeight)
{
	m_vp[0] = 0;
	m_vp[1] = 0;
	m_vp[2] = (float)nWidth;
	m_vp[3] = (float)nHeight;

	if (camera->IsOrtho())
	{
		float fHeight = camera->GetHeight();
		float fWidth = fHeight * nWidth / nHeight;
		m_matProjection.SetOrtho(-fWidth, fWidth, -fHeight, fHeight, camera->GetNear(), camera->GetFar());
	}
	else
		m_matProjection.SetPerspectiveLH(camera->GetFov(), camera->GetAspectRatio(), camera->GetNear(), camera->GetFar());
	
	m_matProjectionInv = m_matProjection;
	m_matProjectionInv.Inverse2();

	// Redundancy LXWordTransformation vs LXCamera 
	m_matView = camera->GetMatrixView();

	m_matVP = m_matProjection * m_matView;
	m_matVPInv = m_matVP;
	m_matVPInv.Inverse2();

	m_matViewInv = m_matView;
	m_matViewInv.Inverse2();

	m_bValid = true;
}

void LXWorldTransformation::MirrorFromCamera(LXActorCamera* pCamera, int nWidth, int nHeight, const vec3f& vOrigin, const vec3f& vUp)
{
	m_vp[0] = 0;
	m_vp[1] = 0;
	m_vp[2] = (float)nWidth;
	m_vp[3] = (float)nHeight;
	m_matProjection.SetPerspectiveLH(pCamera->GetFov(), pCamera->GetAspect(), pCamera->GetNear(), pCamera->GetFar());
	
	m_matProjectionInv = m_matProjection;
	m_matProjectionInv.Inverse();
	
	// Redundancy LXWordTransformation vs LXCamera 
	m_matView = pCamera->GetMatrixView(); 
	m_matViewInv = m_matView;
	m_matViewInv.Inverse();
	
	LXMatrix mirror;
	vec3f vScale = vec3f(1.0,1.0,1.0) - 2.f * vUp;
	mirror.SetScale(vScale);

	LXMatrix trs;
	vec3f vTranslation = vUp * vOrigin * 2.f;
	trs.SetTranslation(vTranslation);
			
	m_matView = m_matView * trs * mirror;
	m_matVP = m_matProjection * m_matView;
	m_bValid = true;
} 

void LXWorldTransformation::GetPickAxis(LXAxis& axis, float x, float y)
{
	CHK(m_bValid);
	if (!m_bValid)
		return;

	y = m_vp[3] - y;
	double upx, upy, upz;
	UnProject(x, y, 0.0f, upx, upy, upz);
	double upx2, upy2, upz2;
	#pragma message("Why 0.5?")
	UnProject(x, y, 0.5f/*1.0f*/, upx2, upy2, upz2);
	axis.SetVector((float)(upx2 - upx), (float)(upy2 - upy), (float)(upz2 - upz));
	axis.SetOrigin((float)upx, (float)upy, (float)upz);
}