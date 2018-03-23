//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

class LXStringA;

enum LXColor
{
	LX_BLACK = 0,
	LX_WHITE = 1
};

template<class T> 
class LXCORE_API LXColor3
{

public:

	// constructors

	LXColor3()								{ }
	LXColor3(T R, T G, T B):r(R),g(G),b(B)	{ }

	// Accessors
			
	void Set(T R, T G, T B)					{ r = R; g = G, b = B; }
	void Set(T RGB)							{ r = RGB; g = RGB; b = RGB; }
	T*   Get()								{ return (T*)this; }
	void SwapRB()							{ T t = r; r = b; b = t; } // TODO : decalage de bit

	// Operators

	bool operator==( const LXColor3& v ) const	{ return ((r == v.r) && (g == v.g) && (b == v.b)); }
	bool operator!=( const LXColor3& v ) const	{ return ((r != v.r) || (g != v.g) || (b != v.b)); }

public:

	T r, g, b;
};


template<class T> 
class LXCORE_API LXColor4
{

public:
	
	// Constructors

	LXColor4( )								{ }
	LXColor4(LXColor eColor)				{ r = (T)eColor; g = (T)eColor; b = (T)eColor; a = (T)eColor;   } // TODO with list
	LXColor4(T tColor)						{ r = tColor; g = tColor; b = tColor; a = 1.0;   }
	LXColor4(T R, T G, T B, T A)			{ r = R; g = G; b = B; a = A; }
	LXColor4(T R, T G, T B)					{ r = R; g = G; b = B; a = 1.0; }
	LXColor4(const LXColor3<T>& c)			{ r = c.r; g = c.g; b = c.b; a = 1.0; }
	LXColor4(const LXColor3<T>& c, T A)		{ r = c.r; g = c.g; b = c.b; a = A; }
	LXColor4(const LXStringA& strHexa);

	LXColor4& operator=(const LXColor4<T>& c)	{ r = c.r; g = c.g; b = c.b; a = c.a; return *this; }

	// Accessors 

	operator T*( ) const					{ return (T*)this;  }
	T*	 Get()								{ return (T*)this; }
	void Set(T R, T G, T B, T A)			{ r = R; g = G, b = B; a = A; }
	void SetHexa(const LXStringA& str);
	
	// Misc

	void Randomize() 
	{ 
		r = (float)rand() / RAND_MAX;
		g = (float)rand() / RAND_MAX;
		b = (float)rand() / RAND_MAX;
	}

	// Operators

	LXColor4 operator+(const LXColor4& v) const { return LXColor4(v.r + r, v.g + g, v.b + b, v.a + a); }
	LXColor4 operator-(const LXColor4& v) const	{ return LXColor4(r - v.r, g - v.g, b - v.b, a - v.a); }
	LXColor4 operator*(const T num) const { return LXColor4(r * num, g * num, b * num, a * num); }

	bool operator==( const LXColor4& v ) const	{ return ((r == v.r) && (g == v.g) && (b == v.b) && (a == v.a)); }
	bool operator!=( const LXColor4& v ) const	{ return ((r != v.r) || (g != v.g) || (b != v.b) || (a != v.a)); }
		
	//LXColor4 operator=(DWORD rgb)			{ CHK(sizeof(T) == 1); return LXColor4(r = GetRValue(rgb), g = GetGValue(rgb), b = GetBValue(rgb), a = 1 ); }
	
	void SwapRB()							{ T t = r; r = b; b = t; }

	T	r, g, b, a;
};

typedef LXColor4<float>					LXColor4f;

// Macros
#define LX_COLOR4F_A	LXColor4f(0.0f, 0.0f, 0.0f, 1.0f)
#define BLACK			LXColor4f(0.0f, 0.0f, 0.0f, 1.0f)
#define GRAY			LXColor4f(0.5f, 0.5f, 0.5f, 1.0f)
#define WHITE			LXColor4f(1.0f, 1.0f, 1.0f, 1.0f)
#define YELLOW			LXColor4f(1.0f, 1.0f, 0.0f, 1.0f)
#define RED				LXColor4f(1.0f, 0.0f, 0.0f, 1.0f)
#define GREEN			LXColor4f(0.0f, 1.0f, 0.0f, 1.0f)
#define BLUE			LXColor4f(0.0f, 0.0f, 1.0f, 1.0f)



