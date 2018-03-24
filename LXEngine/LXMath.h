//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXVec2.h"
#include "LXVec3.h"
#include "LXMacros.h"

#define LX_PI			 3.14159265358979323846f    // PI
#define LX_2PI			 6.283185307179586f			// PI x 2
#define LX_PI_2			 1.57079632679489661923f	// PI / 2
#define LX_PI_4			 0.78539816339744830962f	// PI / 4
#define LX_DEGTORAD(d)	 d*LX_PI/180.0f
#define LX_RADTODEG(r)	 r*180.0f/LX_PI
#define LX_LENSDIAG		 43.2666f					// 24x36 Diagonal lens in mm
#define LX_SMALL_NUMBER4 0.0001f					// 1.e-4f				
#define LX_SMALL_NUMBER6 0.000001f					// 1.e-6f				
#define LX_SMALL_NUMBER8 0.00000001f				// 1.e-8f				

//------------------------------------------------------------------------------------------------------
// Distance helper macros
//------------------------------------------------------------------------------------------------------

__forceinline float KM2CM(float km)
{
	return km * 100000.f;
}

__forceinline float CM2KM(float cm)
{
	return cm / 100000.f;
}

__forceinline float CM2M(float cm)
{
	return cm / 100.f;
}

__forceinline float M2CM(float M)
{
	return M * 100.f;
}

//------------------------------------------------------------------------------------------------------
// Misc
//------------------------------------------------------------------------------------------------------

// Is power of two
__forceinline bool IsPOT(uint i)
{
	return !((i - 1) & i);
}

// Returns the upper power of two of the given value
int UpperPO2(int a);

// Returns the sign (-1 or 1) of the parameter
template <typename T> 
__forceinline int Sign(T val)
{
	return (T(0) < val) - (val < T(0));
}

// Float Functions
__forceinline float LXMin(float v0, const float v1) { return v0<v1?v0:v1; }
__forceinline float LXMax(float v0, const float v1) { return v0>v1?v0:v1; }
__forceinline int LXMax(int a, int b) { return (a > b) ? a : b; }

// Returns the value clamped between min and max.
__forceinline float Clamp(float v, float vMin, float vMax) 
{ 
	return LXMin(LXMax(v, vMin), vMax); 
}

// Hermite Interpolation
__forceinline float SmoothStep( float edge0, float edge1, float v)
{
	float  t = Clamp((v - edge0) / (edge1 - edge0), 0.0f, 1.0f);
	return t * t * (3.0f - 2.0f * t); 
}

// Linear interpolation
__forceinline float Mix(float x, float y, float a)
{
	return x*(1.f - a) + y*a;
}

// Return a random float between 0. and 1.
__forceinline float RandomFloat()
{
	float r = (float)rand() / RAND_MAX;
	CHK(r>=0.f && r<=1.f);
	return r;
}

// Return a random int <= RAND_MAX
__forceinline int RandomInt(int nMax)
{
	CHK(nMax <= RAND_MAX); 
	return rand() % (int)nMax;
}

//------------------------------------------------------------------------------------------------------
// float
//------------------------------------------------------------------------------------------------------

__forceinline bool IsEqual(float a, float b, float epsilon = LX_SMALL_NUMBER6)
{
	float diff = abs(a - b);
	return diff < epsilon;
}

__forceinline bool IsNull(float x)
{
	return IsEqual(x, 0.f);
}

//------------------------------------------------------------------------------------------------------
// Vec3f
//------------------------------------------------------------------------------------------------------

__forceinline vec3f Abs(const vec3f& v)
{
	return vec3f(abs(v.x), abs(v.y), abs(v.z));
}

__forceinline bool IsEqual(const vec3f& a, const vec3f& b, float epsilon = LX_SMALL_NUMBER6)
{
	vec3f diff = Abs(a - b);
	return (diff.x < epsilon) && (diff.y < epsilon) && (diff.z < epsilon);
}

//------------------------------------------------------------------------------------------------------
// Vec2f
//------------------------------------------------------------------------------------------------------

__forceinline float Dot(const vec2f& v0, const vec2f& v1) { return v0.x*v1.x + v0.y*v1.y; }
__forceinline double Dot(const vec2d& v0, const vec2d& v1) { return v0.x*v1.x + v0.y*v1.y; }
__forceinline vec2f sin(vec2f f) { return vec2f(sin(f.x), sin(f.y)); }
__forceinline float Dot(vec2f x, float y) { return Dot(x, vec2f(y)); }

__forceinline vec2f fract(const vec2f& v) { float intpart; return vec2f(modf(v.x, &intpart), modf(v.y, &intpart)); }
__forceinline vec2d fract(const vec2d& v) { double intpart; return vec2d(modf(v.x, &intpart), modf(v.y, &intpart)); }

__forceinline vec2f floor(const vec2f& v) { 	return vec2f(floor(v.x), floor(v.y)); }
__forceinline vec2d floor(const vec2d& v) { 	return vec2d(floor(v.x), floor(v.y)); }

//------------------------------------------------------------------------------------------------------
// Vec3
//------------------------------------------------------------------------------------------------------

__forceinline vec3f LXMax(vec3f x, float y) { 	return vec3f(LXMax(x.x, y), LXMax(x.y, y), LXMax(x.z, y));  }
__forceinline vec3f fract(const vec3f& v) { float intpart; return vec3f(modf(v.x, &intpart), modf(v.y, &intpart), modf(v.z, &intpart)); }

// Generate normalized vectors
void GenerateRandomVectors(int Count, vector<vec3f>& Vectors);
