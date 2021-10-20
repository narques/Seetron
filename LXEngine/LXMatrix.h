//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXExports.h"
#include "LXMacros.h"
#include "LXVec3.h"
#include "LXVec4.h"

class LXAxis;
class LXBBox;

LX_ALIGN(16) class LXENGINE_API LXMatrix
{
	
	friend LXMatrix Inverse(const LXMatrix& matrix);
	friend LXMatrix Transpose(const LXMatrix& matrix); 
	friend class LXActor;

public:

	LXMatrix();
	LXMatrix(const float pFloat[16]) { memcpy(m_fData, pFloat, sizeof(m_fData)); }
	LXMatrix(const LXMatrix& m) { memcpy(m_fData, m.m_fData, sizeof(m_fData)); }
	~LXMatrix();
	
	bool			operator!=		( LXMatrix& m) const ;
	bool			operator!=		( const LXMatrix& m) const ;
	bool			operator==		( const LXMatrix& m ) const;

	LXMatrix&		operator=		( const LXMatrix& m ) { memcpy(m_fData, m.m_fData, sizeof(m_fData)); return *this; }
	LXMatrix&		operator=		( float* pFloat) { memcpy(m_fData, pFloat, sizeof(m_fData)); return *this; }
	float&			operator[]		( unsigned int i ) { CHK(i<16); return m_fData[i]; }
	const float&	operator[]		( unsigned int i ) const { CHK(i<16); return m_fData[i]; }
		
	const LXMatrix	operator*		( const LXMatrix& matrix ) const;
	const vec3f		operator*		( const vec3f& v ) const;
	const LXBBox	operator*		( const LXBBox& v) const;

	LXMatrix&		operator*=		( const LXMatrix& matrix );

	const LXMatrix	MultHomogenic	( const LXMatrix& matrix ) const;
	const LXMatrix	MultProj		( const LXMatrix& matrix ) const;
			
	const float*	GetPtr			( ) const { return m_fData; }
	
	bool			IsIdentity		( ) const;
	bool			IsRightHanded	( ) const;
	
	void			Inverse			( );
	void			Inverse2		( );
	void			SetIdentity		( );
	
	//
	// Composition
	//

	// Projection
	void			SetOrtho		( float left, float right, float bottom, float top, float near, float far );
	void			SetPerspectiveLH(float fovy, float aspect, float zNear, float zFar);
	void			SetPerspectiveRH(float fovy, float aspect, float zNear, float zFar);
	void			BuildInfinitePerspectiveLH(float fovy, float aspect, float zNear);
	void			BuildInfinitePerspectiveRH(float fovy, float aspect, float zNear);

	// Misc
	void			SetOrigin	( float x, float y, float z) { SetOrigin(vec3f(x,y,z)); };
	void			SetOrigin	( const vec3f& vo );
		
	// Quaternion
	void			SetRotation ( float angle, float x, float y, float z );  
	
	// Euler
	void			SetRotation	( const vec3f& vEulerAngles ) { SetRotation(vEulerAngles.x, vEulerAngles.y, vEulerAngles.z); }
	void			SetRotation	( float x, float y, float z );						

	void			SetScale	( const vec3f& vScale ) { SetScale(vScale.x, vScale.y, vScale.z); }
	void			SetScale	( float x, float y, float z );

	void			SetTranslation ( const vec3f& vTranslation ) { SetTranslation(vTranslation.x, vTranslation.y, vTranslation.z); }
	void			SetTranslation ( float x, float y, float z );
	
	void			SetXYZ		( const vec3f& vx, const vec3f& vy, const vec3f& vz );
	void			SetVz		( const vec3f& vz, bool bDoOrthonormal = true );
	
	void			SetAt		( int nRow, int nCol,  float value ) { 	m_fData[nRow * 4 + nCol] = value; 	}
	
	// --------------------------------------------------------------------------------------------------------------
	// Modification
	// --------------------------------------------------------------------------------------------------------------

	void			Translate	(  const vec3f& vTranslation )
	{
		m_fData[12] += vTranslation.x;
		m_fData[13] += vTranslation.y;
		m_fData[14] += vTranslation.z;
	}

	void			Rotate		( float fAngle, const vec3f& vAxis);
	void			RotateX		( float phi );
	void			RotateY		( float phi );
	void			RotateZ		( float phi );
	
	
	// --------------------------------------------------------------------------------------------------------------
	// Decomposition
	// --------------------------------------------------------------------------------------------------------------

	vec3f			GetEulerAngles	( ) const; // Euler angles in degree
	vec3f			GetScale		( ) const;

	// --------------------------------------------------------------------------------------------------------------
	// Basic Gets
	// --------------------------------------------------------------------------------------------------------------

	const vec3f&	GetOrigin	( )	const { return *(vec3f*)&m_fData[12];  }
	vec3f			GetVx		( ) const { return vec3f(m_fData[0], m_fData[1], m_fData[2]); }
	vec3f			GetVy		( ) const { return vec3f(m_fData[4], m_fData[5], m_fData[6]); }
	vec3f			GetVz		( ) const { return vec3f(m_fData[8], m_fData[9], m_fData[10]); }

	vec4f			GetRow		( unsigned int i ) const;
	
	// --------------------------------------------------------------------------------------------------------------
	// Transformations
	// --------------------------------------------------------------------------------------------------------------

	void			LocalToParentPoint	( vec3f& point ) const;
	void			LocalToParentPoint	( vec4f& point ) const;
	void			LocalToParentVector	( vec3f& vector ) const;
	void			LocalToParent		( LXBBox& bbox ) const;
	void			LocalToParent		( LXAxis& axis ) const;
	
	void			ParentToLocalPoint	( vec3f& point )  const;
	void			ParentToLocalPoint	( vec4f& point )  const;
	void			ParentToLocalVector	( vec3f& vector ) const;
	void			ParentToLocal		( LXBBox& bbox ) const; 
	void			ParentToLocal		( LXAxis& axis ) const;
	void			ParentToLocal		( LXMatrix& Matrix ) const;

	// --------------------------------------------------------------------------------------------------------------
	// Misc
	// --------------------------------------------------------------------------------------------------------------

	void FlipHand();
		
public:

	union 
	{
		struct
		{
			float m_fData[16];			/* 1 2 3 4 5 6 ... */
		};
		struct
		{
			float m_44[4][4];           /* m[Column][Row] m44[0][...] ( 1 2 3 4)   m44[1][...] (5 6 7 8) */
		};
		struct  
		{
			float m11, m12, m13, m14;   /* 1 2 3 4 5 6 ... */
			float m21, m22, m23, m24;
			float m31, m32, m33, m34;
			float m41, m42, m43, m44;
		};
	};
};

LXMatrix Inverse(const LXMatrix& matrix);
LXMatrix Transpose(const LXMatrix& matrix);
