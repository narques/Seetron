//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXObject.h"
#include <Windows.h>

class LXCORE_API LXPerformance : public LXObject
{

public:

	LXPerformance(void);
	virtual ~LXPerformance(void);

	void		Reset		( );
	double		GetTime		( ) const;
	LONGLONG	GetCycles	( ) const;
	
private:

	LARGE_INTEGER m_frequency;
	LARGE_INTEGER m_counter;
	
};

