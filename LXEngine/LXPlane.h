//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXVec3.h"

class LXPlane
{

	friend class LXFrustum;

public:

	LXPlane( void );
	LXPlane( const float x, const float y, const float z, const float w );
	LXPlane(const vec3f& v0, const vec3f& v1, const vec3f& v2);
	~LXPlane( );

	// Accessors
	void Set( const vec3f& v0, const vec3f& v1, const vec3f& v2 );
	void Set( const float x, const float y, const float z, const float w );
	const vec3f& Normal() const { return *m_vNormal; }
	void Normalize( );
	float Distance( const vec3f &v ) const;

private:

	vec3f*	m_vNormal;
	float	m_fDistance;

};

