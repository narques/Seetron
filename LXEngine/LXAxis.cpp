//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "pch.h"
#include "LXAxis.h"
#include "LXMath.h"

LXAxis::LXAxis()
{
  _vOrigin.Set(0.0f, 0.0f, 0.0f);
  _vVector.Set(1.0f, 0.0f, 0.0f);
}

LXAxis::LXAxis( const LXAxis& axis )
{
	operator=(axis);
}

LXAxis::~LXAxis(void)
{
}

float LXAxis::Distance( LXAxis& axis, vec3f* pI1, vec3f* pI2) const
{
	vec3f v1 = _vVector;
	vec3f v2 = axis.GetVector();

	v1.Normalize();
	v2.Normalize();

	const vec3f& p1 = _vOrigin;
	const vec3f& p2 = axis.GetOrigin();
	const vec3f c = p2-p1;

	const float a1 =  c.DotProduct(v1);
	const float a2 = c.DotProduct(v2) * -1.0f;
	
	const float b = v1.DotProduct(v2);
  
	const float div = (1 - b*b);
  
	if (IsNearlyZero(div))
	{
		return -1.f;
	}

	const float k1 = (a1 + b*a2) / div;
	const float k2 = (a2+b*k1);

	const vec3f i1 = p1 + v1*k1;
	const vec3f i2 = p2 + v2*k2;

	if (pI1)
	{
		*pI1 = i1;
	}

	if (pI2)
	{
		*pI2 = i2;
	}

	return i1.Distance(i2);
}

bool LXAxis::IntersectPlane( const LXAxis& plane, vec3f* outIntersection) const
{
	const vec3f& n = plane.GetVector();
	const vec3f& dir = GetVector();
	const vec3f w0 = GetOrigin() - plane.GetOrigin();
	const float a = -n.DotProduct(w0);
	const float b = n.DotProduct(dir);
	
	if (IsNearlyZero(b)) 
	{
		return false;    
	}

	const float r = a / b;
	
	if (r < 0.0)                  
	{
		return false;                
	}
	
	*outIntersection = GetOrigin() + r * dir;           
	return true;
}
