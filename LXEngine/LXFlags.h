//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXTypes.h"

template<class T>
struct LXCORE_API LXFlags
{

public:

	// Constructors
	LX_INLINE LXFlags(void)							{ }
	LX_INLINE LXFlags(T t)							{ Flags = (int)t; }
	LX_INLINE LXFlags(int t)						{ Flags = (int)t; }
	
	// Accessors
	LX_INLINE operator int() const					{ return Flags; }

	// Operators
	LX_INLINE LXFlags&	operator |= ( T t )			{ return (LXFlags&)(Flags |= (int)t); }
	LX_INLINE LXFlags	operator &  ( T t ) const	{ return (LXFlags)(Flags & (int)t); }		// Not inline if destructor is uncommented. Strange !
	
private:

	int Flags;
};