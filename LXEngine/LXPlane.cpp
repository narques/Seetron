//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "pch.h"
#include "LXPlane.h"
#include "LXEngine.h"

LXPlane::LXPlane(void)
{
	m_vNormal = new vec3f();
}

LXPlane::~LXPlane(void)
{
	delete 	m_vNormal;
}

void LXPlane::Set(const float x, const float y, const float z, const float w)
{
	m_vNormal->Set(x, y, z);
	m_fDistance = w;
}

LXPlane::LXPlane(const float x, const float y, const float z, const float w)
{
	m_vNormal = new vec3f(x, y, z);
	m_fDistance = w;
}

LXPlane::LXPlane(const vec3f& v0, const vec3f& v1, const vec3f& v2)
{
	m_vNormal = new vec3f();
	(*m_vNormal) = CrossProduct(v1 - v0, v2 - v0);
	m_vNormal->Normalize();
	m_fDistance = -(m_vNormal->DotProduct(v0));
}

void LXPlane::Set(const vec3f& v0, const vec3f& v1, const vec3f& v2)
{
	(*m_vNormal) = CrossProduct(v1 - v0, v2 - v0);
	m_vNormal->Normalize();
	m_fDistance = -(m_vNormal->DotProduct(v0));
}

void LXPlane::Normalize()
{
	float invLen = 1.0f / m_vNormal->Length();
	*m_vNormal *= invLen;
	m_fDistance *= invLen;
}

float LXPlane::Distance(const vec3f &v) const 
{
	float f = m_vNormal->DotProduct(v) + m_fDistance;
	return f;
}
