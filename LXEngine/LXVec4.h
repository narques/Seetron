//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------


#pragma once

#include "LXColor.h"
#include "LXVec3.h"

template<class T>
class vec4
{

public:

	// Constructors

	LX_INLINE vec4( )													{ }
	LX_INLINE vec4( T X, T Y, T Z, T W ):x(X),y(Y),z(Z),w(W)			{ }
	LX_INLINE vec4( const vec4& v ):x(v.x),y(v.y),z(v.z),w(v.w)			{ }
	LX_INLINE vec4( const vec3<T>& v, T W ):x(v.x),y(v.y),z(v.z),w(W)	{ }
	LX_INLINE vec4( const LXColor4<T>& c ):x(c.r),y(c.g),z(c.b),w(c.a) 	{ }
		
	// Accessors

	LX_INLINE operator const T*() const									{ return (T*)this; }
	LX_INLINE const T& operator[ ]	(unsigned int i)					{ return *(&x + i); }
	
	LX_INLINE void Set(T X, T Y, T Z, T W)								{ x = X; y = Y; z = Z; w = W; }
	LX_INLINE const vec3<T>&	xyz() const								{ return (vec3<T>&)*this; }
	LX_INLINE operator const vec3<T>&() const							{ return (vec3<T>&)*this; }
	
	// Operators
	
	LX_INLINE vec4		operator+(const vec4& v) const					{ return vec4(v.x + x, v.y + y, v.z + z, v.w + w); }
	LX_INLINE vec4		operator-(const vec4& v) const					{ return vec4(x - v.x, y - v.y, z - v.z, v.w - w); }
	LX_INLINE vec4		operator*(const vec4& v) const					{ return vec4(x * v.x, y * v.y, z * v.z, v.w * w); }
	LX_INLINE vec4		operator/(const vec4& v) const					{ return vec4(x / v.x, y / v.y, z / v.z, v.w / w); }

	LX_INLINE vec4		operator+(const T num) const					{ return vec4(x + num, y + num, z + num, w + num); }
	LX_INLINE vec4		operator-(const T num) const					{ return vec4(x - num, y - num, z - num, w - num); }
	LX_INLINE vec4		operator*(const T num) const					{ return vec4(x * num, y * num, z * num, w * num); }
	LX_INLINE vec4		operator/(const T num) const  					{ return vec4(x / num, y / num, z / num, w / num); }

	LX_INLINE vec4&		operator=(const vec4& v)						{ x = v.x; y = v.y; z = v.z; w = v.w; return *this; }

	LX_INLINE vec4&		operator+=(vec4& v)								{ x += v.x; y += v.y; z += v.z; w += v.w; return *this; }
	LX_INLINE vec4&		operator-=(vec4& v)								{ x -= v.x; y -= v.y; z -= v.z; w -= v.w; return *this; }
	LX_INLINE vec4&		operator*=(vec4& v)								{ x *= v.x; y *= v.y; z *= v.z; w *= v.w; return *this; }
	LX_INLINE vec4&		operator/=(vec4& v)								{ x /= v.x; y /= v.y; z /= v.z; w /= v.w; return *this; }

	LX_INLINE vec4&		operator+=(T v)									{ x += v; y += v; z += v; w += v; return *this; }
	LX_INLINE vec4&		operator-=(T v)									{ x -= v; y -= v; z -= v; w -= v; return *this; }
	LX_INLINE vec4&		operator*=(T v)									{ x *= v; y *= v; z *= v; w *= v; return *this; }
	LX_INLINE vec4&		operator/=(T v)									{ x /= v; y /= v; z /= v; w /= v; return *this; }

	// Comparison Operators

	LX_INLINE bool		operator!=( const vec4& v ) const				{ return ((x != v.x) || (y != v.y) || (z != v.z) || (w != v.w)); }
	LX_INLINE bool		operator==( const vec4& v ) const				{ return ((x == v.x) && (y == v.y) && (z == v.z) || (w == v.w)); }

	// Math

	LX_INLINE T			Length() const									{ return sqrt(x*x + y*y + z*z + w*w); }

public:

	T x, y, z, w;
};

typedef vec4<float>		vec4f;
typedef vector<vec4f>	ArrayVec4f;

// Functions & Operators
LX_INLINE static vec4f	operator*		( float num, const vec4f& v )	{ return vec4f(v.x * num, v.y * num, v.z * num, v.w * num); }

// Macros
#define LX_VEC4F_NULL vec4f(0.f, 0.f, 0.f, 0.f)

//Explicit class Instantiation
template class vec4<float>;

