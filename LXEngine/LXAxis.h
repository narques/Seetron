//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXObject.h"
#include "LXVec3.h"

class LXCORE_API LXAxis : public LXObject
{

public:
	
	LXAxis();
	LXAxis( const LXAxis& );
	LXAxis( vec3f& vOrigin, vec3f& vVector ){ _vOrigin = vOrigin; _vVector = vVector; }
	virtual ~LXAxis();

	LXAxis& operator= (const LXAxis& a)
	{
		_vOrigin = a._vOrigin;
		_vVector = a._vVector;
		return *this;
	}

	void Set( const vec3f& vOrigin, const vec3f& vVector ) { _vOrigin = vOrigin; _vVector = vVector; }
	void SetOrigin( const vec3f& v )				{ _vOrigin = v;}
	void SetOrigin( float x, float y, float z )		{ _vOrigin.Set(x,y,z); }
	const vec3f& GetOrigin( ) const					{ return _vOrigin; }
	void SetVector( const vec3f& v )				{ _vVector = v; _vVector.Normalize(); }
	void SetVector( float x, float y, float z )		{ _vVector.Set(x,y,z); _vVector.Normalize(); }
	const vec3f& GetVector( ) const					{ return _vVector; }
  
	float Distance( LXAxis& axis, vec3f* pI1=nullptr, vec3f* pI2=nullptr) const;
	bool IntersectPlane( const LXAxis& axis, vec3f* outIntersection) const;
  
private:

	vec3f		_vOrigin;
	vec3f		_vVector;
  
};
