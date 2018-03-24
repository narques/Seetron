//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

template<class T>
class LXCORE_API ivec3
{

public:

	LX_INLINE ivec3( )													{ }
	LX_INLINE ivec3( T X, T Y, T Z ):x(X),y(Y),z(Z)						{ }
	LX_INLINE operator const T*( ) const								{ return (T*)this; }
	T x,y,z;
};

typedef ivec3<uint>				vec3ui;
typedef vec3ui					CFace3;
typedef vector<vec3ui>			ArrayVec3ui;
typedef list<CFace3*>			ListFaces;

// Explicit class Instantiation

template class LXCORE_API ivec3<uint>;