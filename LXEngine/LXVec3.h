//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

template<class T> 
class vec3
{

public:

	// Constructors

	__forceinline vec3( )												{ }
	__forceinline vec3( T X ) :x(X), y(X), z(X)							{ }
	__forceinline vec3( T X, T Y, T Z ):x(X),y(Y),z(Z)					{ }
	__forceinline vec3( const vec3& v ):x(v.x),y(v.y),z(v.z)			{ }
	
	// Accessors
	
	__forceinline operator const T*( ) const							{ return (T*)this; }
	__forceinline const T& operator[ ]	( unsigned int i )				{ return *(&x + i); }
	__forceinline void	Set( const vec3& v )							{ x = v.x; y = v.y, z = v.z; }
	__forceinline void	Set( T X, T Y, T Z )							{ x = X; y = Y, z = Z; }
	__forceinline void	SetMax( const vec3& a, const vec3& b )			{ x = a.x>b.x?a.x:b.x;  y = a.y>b.y?a.y:b.y;  z = a.z>b.z?a.z:b.z; 	}
	__forceinline void	SetMin( const vec3& a, const vec3& b )			{ x = a.x<b.x?a.x:b.x; 	y = a.y<b.y?a.y:b.y;  z = a.z<b.z?a.z:b.z;  }

	// Operators

	__forceinline vec3	operator+(const vec3& v) const					{ return vec3(v.x + x, v.y + y, v.z + z); }
	__forceinline vec3	operator-(const vec3& v) const					{ return vec3(x - v.x, y - v.y, z - v.z); }
	__forceinline vec3	operator*(const vec3& v) const					{ return vec3(x * v.x, y * v.y, z * v.z); }
	__forceinline vec3	operator/(const vec3& v) const					{ return vec3(x / v.x, y / v.y, z / v.z); }

	__forceinline vec3	operator+(const T num) const					{ return vec3(x + num, y + num, z + num); }
	__forceinline vec3	operator-(const T num) const					{ return vec3(x - num, y - num, z - num); }
	__forceinline vec3	operator*(const T num) const					{ return vec3(x * num, y * num, z * num); }
	__forceinline vec3	operator/(const T num) const  					{ return vec3(x / num, y / num, z / num); }

	__forceinline vec3&	operator=(const vec3& v)						{ x = v.x; y = v.y; z = v.z; return *this; }

	__forceinline vec3&	operator+=(const vec3& v)						{ x += v.x; y += v.y; z += v.z; return *this; }
	__forceinline vec3&	operator-=(vec3& v)								{ x -= v.x; y -= v.y; z -= v.z; return *this; }
	__forceinline vec3&	operator*=(vec3& v)								{ x *= v.x; y *= v.y; z *= v.z; return *this; }
	__forceinline vec3&	operator/=(vec3& v)								{ x /= v.x; y /= v.y; z /= v.z; return *this; }

	__forceinline vec3&	operator+=(T v)									{ x += v; y += v; z += v; return *this; }
	__forceinline vec3&	operator-=(T v)									{ x -= v; y -= v; z -= v; return *this; }
	__forceinline vec3&	operator*=(T v)									{ x *= v; y *= v; z *= v; return *this; }
	__forceinline vec3&	operator/=(T v)									{ x /= v; y /= v; z /= v; return *this; }

	__forceinline bool operator!=( const vec3& v ) const				{ return ((x != v.x) || (y != v.y) || (z != v.z)); }
	__forceinline bool operator==( const vec3& v ) const				{ return ((x == v.x) && (y == v.y) && (z == v.z)); }
	__forceinline bool operator<( const vec3& v ) const				
	{ 
		if (x<v.x)
			return true;

		if (x==v.x)
		{
			if (y<v.y)
				return true;

			if (y==v.y)
			{
				if(z<v.z)
					return true;
			}
		}

		return false;
	}

	// Math
	
	__forceinline T			Length() const									{ return sqrt(x*x + y*y + z*z); }
	__forceinline T			Distance( const vec3& v) const					{ return (T)sqrt((float)((x-v.x)*(x-v.x)+(y-v.y)*(y-v.y)+(z-v.z)*(z-v.z))); }
	__forceinline void		Normalize( )									{ T l = (T)sqrt(x*x + y*y + z*z);  x/=l; y/=l; z/=l;  }
	__forceinline T			DotProduct( const vec3& v ) const				{ return x*v.x + y*v.y + z*v.z; }
	__forceinline void		CrossProduct( const vec3& u, const vec3& v)		{ x = u.y * v.z - u.z * v.y;  y = u.z * v.x - u.x * v.z; z = u.x * v.y - u.y * v.x; }
	
	// Misc
	
	__forceinline vec3		yzx() const { return vec3(y, z, x); }

	__forceinline bool		IsNull( )										{ return (x == 0.f && y == 0.f && z == 0.f); }

public:

	T x, y, z;

};

typedef vec3<float>		vec3f;
typedef vec3<double>	vec3d;

typedef std::vector<vec3f>	ArrayVec3f;
typedef std::vector<vec3d>	ArrayVec3d;

// Specialization
// Functions & Operators
__forceinline static vec3f	CrossProduct( const vec3f& v0, const vec3f& v1 )			{ vec3f vr = v0; vr.CrossProduct(v0, v1); return vr; }
__forceinline static float  DotProduct( const vec3f& v0, const vec3f& v1)				{ return v0.x*v1.x + v0.y*v1.y + v0.z*v1.z; }
__forceinline static float  Dot( const vec3f& v0, const vec3f& v1)						{ return v0.x*v1.x + v0.y*v1.y + v0.z*v1.z; }
__forceinline static vec3f	operator*( float num, const vec3f& v )						{ return vec3f(v.x * num, v.y * num, v.z * num); }
__forceinline static vec3f	operator-(float num, const vec3f& v)						{ return vec3f(num - v.x, num - v.y, num - v.z); }
__forceinline static vec3f  Normalize( const vec3f& v )									
{ 
	float  l = sqrtf(v.x*v.x + v.y*v.y + v.z*v.z); 
	return vec3f(v.x/l, v.y/l, v.z/l);  
}

__forceinline static float	CosAngle( const vec3f& v0, const vec3f& v1 )			
{ 
	float f = sqrtf((v0.x*v0.x + v0.y*v0.y + v0.z*v0.z)*(v1.x*v1.x + v1.y*v1.y + v1.z*v1.z)) ;
	return v0.DotProduct(v1) / f;
}

__forceinline static bool IsValid( const vec3f& v )										{ return !_isnanf(v.x) && !_isnanf(v.y) && !_isnanf(v.z); }

// Macros
#define LX_VEC3F_NULL vec3f(0.f, 0.f, 0.f)
#define LX_VEC3F_X vec3f(1.f, 0.f, 0.f)
#define LX_VEC3F_Y vec3f(0.f, 1.f, 0.f)
#define LX_VEC3F_Z vec3f(0.f, 0.f, 1.f)
#define LX_VEC3F_NX vec3f(-1.f, 0.f, 0.f)
#define LX_VEC3F_NY vec3f(0.f, -1.f, 0.f)
#define LX_VEC3F_NZ vec3f(0.f, 0.f, -1.f)
#define LX_VEC3F_XYZ vec3f(1.f, 1.f, 1.f)
#define LX_VEC3F_NXYZ vec3f(-1.f, -1.f, -1.f)
#define LX_VEC3F_XYZ_50 vec3f(50.f, 50.f, 50.f)
#define LX_VEC3F_NXYZ_50 vec3f(-50.f, -50.f, -50.f)
