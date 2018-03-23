//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXVec3.h"

template<class T> 
class LXCORE_API vec2
{

public:

	// Constructors

	__forceinline vec2()													{ }
	__forceinline vec2(T X) :x(X), y(X)										{ }
	__forceinline vec2(T X, T Y):x(X),y(Y)									{ }
	__forceinline vec2( const vec2& v ):x(v.x),y(v.y)						{ }

	// Accessors

	__forceinline T*            Get( )										{ return (T*)this; }
	__forceinline void          Set( T X, T Y )								{ x=X; y=Y;}

	// Operators

	__forceinline vec2		operator+(const vec2& v) const					{ return vec2(v.x + x, v.y + y); }
	__forceinline vec2		operator-(const vec2& v) const					{ return vec2(x - v.x, y - v.y); }
	__forceinline vec2		operator*(const vec2& v) const					{ return vec2(x * v.x, y * v.y); }
	__forceinline vec2		operator/(const vec2& v) const					{ return vec2(x / v.x, y / v.y); }

	__forceinline vec2		operator+(const T num) const					{ return vec2(x + num, y + num); }
	__forceinline vec2		operator-(const T num) const					{ return vec2(x - num, y - num); }
	__forceinline vec2		operator*(const T num) const					{ return vec2(x * num, y * num); }
	__forceinline vec2		operator/(const T num) const  					{ return vec2(x / num, y / num); }

	__forceinline vec2&		operator=(const vec2& v)						{ x = v.x; y = v.y; return *this; }

	__forceinline vec2&		operator+=(const vec2& v)						{ x += v.x; y += v.y; return *this; }
	__forceinline vec2&		operator-=(const vec2& v)						{ x -= v.x; y -= v.y; return *this; }
	__forceinline vec2&		operator*=(const vec2& v)						{ x *= v.x; y *= v.y; return *this; }
	__forceinline vec2&		operator/=(const vec2& v)						{ x /= v.x; y /= v.y; return *this; }

	__forceinline vec2&		operator+=(T v)									{ x += v; y += v; return *this; }
	__forceinline vec2&		operator-=(T v)									{ x -= v; y -= v; return *this; }
	__forceinline vec2&		operator*=(T v)									{ x *= v; y *= v; return *this; }
	__forceinline vec2&		operator/=(T v)									{ x /= v; y /= v; return *this; }
	
	// Comparison Operators

	__forceinline bool		operator!=( const vec2& v ) const				{ return ((x != v.x) || (y != v.y)); }
	__forceinline bool		operator==( const vec2& v ) const				{ return ((x == v.x) && (y == v.y)); }

	// Math
	
	__forceinline T		Distance( const vec2& v) const						{ return (T)sqrt((float)((x-v.x)*(x-v.x)+(y-v.y)*(y-v.y))); }
	__forceinline void	Normalize( )										{ T l = (T)sqrt(x*x + y*y); x/=l; y/=l; }
	__forceinline T		Length( ) const										{ return sqrtf(x*x + y*y); } //TODO SPECILATSATION

	// Misc

	vec3<T> xyx() const { return vec3<T>(x, y, x); }
		
public:
	
	T x,y;
};

typedef vec2<float>		vec2f;
typedef vec2<double>	vec2d;
typedef vector<vec2f>	ArrayVec2f;

// Functions & Operators
__forceinline static vec2f	operator*(float num, const vec2f& v) { return vec2f(num * v.x, num * v.y); }
__forceinline static vec2f	operator+(float num, const vec2f& v) { return vec2f(num + v.x, num + v.y); }
__forceinline static vec2f	operator-(float num, const vec2f& v) { return vec2f(num - v.x, num - v.y); }

__forceinline static vec2d	operator*(double num, const vec2d& v) { return vec2d(num * v.x, num * v.y); }
__forceinline static vec2d	operator+(double num, const vec2d& v) { return vec2d(num + v.x, num + v.y); }
__forceinline static vec2d	operator-(double num, const vec2d& v) { return vec2d(num - v.x, num - v.y); }
