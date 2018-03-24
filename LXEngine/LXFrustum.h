//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXObject.h"
#include "LXPlane.h"

class LXMatrix;

enum class EFrustumTestResult
{
	Inside,
	Outside,
	Cut
};

enum class EViewFrustumPlane
{
	Left,
	Right,
	Front,
	Back,
	top,
	Down
};

class LXCORE_API LXFrustum : public LXObject
{

public:

	LXFrustum(void);
	virtual ~LXFrustum(void);

	void			Update			( const LXMatrix& mvp );
	void			Update			( vec3f* pPoints );

	bool			IsBoxIn			( const vec3f& v0, const vec3f& v1 ) const { 	return IsBoxIn( v0.x, v0.y, v0.z, v1.x, v1.y, v1.z); }
	bool			IsBoxIn			( float x, float y, float z, float x2, float y2, float z2 ) const ;
	
	bool			IsSphereIn		( const vec3f& vPoint, float radius ) const;
	bool			IsSphereIn		( const vec3f& vPoint, float radius, EFrustumTestResult& ftr ) const;

	bool            IsPointIn       ( float x, float y, float z) const;
	bool			IsTriangleIn	( const vec3f& v0, const vec3f& v1, const vec3f& v2 ) const;
	
protected:

	LXPlane			m_Frustum[6];
};

