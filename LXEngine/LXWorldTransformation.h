//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXObject.h"
#include "LXMatrix.h"

class LXBBox;
class LXActorCamera;
class LXAxis;

class LXCORE_API LXWorldTransformation : public LXObject
{

public:

	LXWorldTransformation(void);
	virtual ~LXWorldTransformation(void);

	LXWorldTransformation&		operator=		( const LXWorldTransformation& wt ) 
	{ 
		m_vp[0] = wt.m_vp[0];
		m_vp[1] = wt.m_vp[1];
		m_vp[2] = wt.m_vp[2];
		m_vp[3] = wt.m_vp[3];
		m_matView = wt.m_matView;
		m_matProjection = wt.m_matProjection;
		m_matVP = wt.m_matVP;
		m_matVPInv = wt.m_matVPInv;
		m_bValid = wt.m_bValid;
		return *this; 
	}

	void			CreateOrtho			( int nWidth, int nHeight ); 
	void			CreateOrtho			( int nWidth, int nHeight, float l, float r, float b, float t, float n, float f );
	void			FromCamera			( LXActorCamera* pCamera,  int nWidth, int nHeight );
	void			MirrorFromCamera	( LXActorCamera* pCamera,  int nWidth, int nHeight, const vec3f& vOrigin, const vec3f& vUp );

	const LXMatrix&	GetMatrixProjection	( ) const { return m_matProjection; }
	const LXMatrix&	GetMatrixView		( ) const { return m_matView; }
	const LXMatrix& GetMatrixVP			( ) const { return m_matVP; }
	
	const LXMatrix&	GetMatrixVPInv		( ) const { return m_matVPInv; }
	const LXMatrix& GetMatrixProjectionInv( ) const { return m_matProjectionInv; }
	const LXMatrix&	GetMatrixViewInv	( ) const { return m_matViewInv; }
	
	void			UnProject			( const double winx, const double winy, const double winz, double& x, double& y, double& z ) const;
	void			UnProject2WCS		( const double winx, const double winy, const double winz, double& upx, double& upy, double& upz ) const ;
	void			UnProject			( vec3f& v );
	void			UnProject2WCS		( vec3f& v );
		
	void			Project				( const double x, const double y, const double z, double& winx, double& winy, double& winz );
	void			Project				( LXBBox* pBox );
	void			Project				( vec3f& v );
	
	void			GetPickAxis			( LXAxis& axis, float x, float y );
		
	void			Invalidate			( ) { m_bValid = false; }
	bool			IsValid				( ) const { return m_bValid; }

	float			Width				( ) const { return m_vp[2];	}
	float			Height				( ) const { return m_vp[3]; }
	
private:

	float			m_vp[4];

	LXMatrix		m_matView;
	LXMatrix		m_matProjection;
	LXMatrix		m_matVP;

	LXMatrix		m_matViewInv;
	LXMatrix		m_matProjectionInv;
	LXMatrix		m_matVPInv;

	bool			m_bValid;
};
