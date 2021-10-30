//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXCore/LXObject.h"
#include "LXTypes.h"
#include "LXVec3.h"
#include "LXVec4.h"

class LXENGINE_API LXBBox : public LXObject
{

public:

	LXBBox				( void );
	LXBBox				( const LXBBox& bbox );
	virtual ~LXBBox		( void );

	LXBBox				operator+(const LXBBox& b) const {  LXBBox r(b); r.Add(*this); return b; }
	LXBBox&				operator=(const LXBBox& m);
		
	void				Reset( );
	
	void				Invalidate( )			{ m_bValid = false; }
	bool				IsValid( ) const		{ return m_bValid; }
	bool				IsPoint( ) const;
	bool				IsLine( ) const;

	// Add methods: Update the Min and Max according a position or a box
	void				Add( const LXBBox& box );
	void				Add( const vec3f& point );

	// Extend methods: Update the Min and Max values according a size;
	void				ExtendZ(float z);
		
	const vec3f&		GetCenter() const		{ CHK(m_bValid); return (vec3f&)m_ptCenter; }
	const vec3f&		GetMin() const			{ CHK(m_bValid); return (vec3f&)m_ptMin; }
	const vec3f&		GetMax() const			{ CHK(m_bValid); return (vec3f&)m_ptMax; }
	float				GetMinSize() const		{ return (std::min)((std::min)(m_ptMin.x, m_ptMin.y), m_ptMin.z); }
	float				GetMaxSize() const		{ return (std::max)((std::max)(m_ptMax.x, m_ptMax.y), m_ptMax.z); }
	void				GetPoints(vec3f* pPoints) const ;
	float	            GetDiag() const			{ CHK(m_bValid); return m_fDiag; }			
	float				GetSizeX() const		{ CHK(m_bValid); return m_ptMax.x - m_ptMin.x; }
	float				GetSizeY() const		{ CHK(m_bValid); return m_ptMax.y - m_ptMin.y; }
	float				GetSizeZ() const		{ CHK(m_bValid); return m_ptMax.z - m_ptMin.z; }
	vec3f				GetSize() const			{ CHK(m_bValid); return vec3f(m_ptMax.x - m_ptMin.x, m_ptMax.y - m_ptMin.y, m_ptMax.z - m_ptMin.z); }

private:

	bool				TryToValidate( );
	void				ComputeCenter();
	void				ComputeDiagonal();

public:

	vec3f				m_ptMax;
	vec3f				m_ptMin;

private:

	float				m_fDiag;
	vec3f				m_ptCenter;
	bool				m_bValid;
	
};

