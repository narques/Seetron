//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXCore/LXObject.h"

class LXInterpolator : public LXObject
{
public:

	LXInterpolator();
	virtual ~LXInterpolator();

	template<class T>
	static void InterpolateLinear(const T& v0, const T& v1, double t, T& result);

	template<class T>
	static void InterpolateSmooth(const T& v0, const T& v1, double t, T& result);

	template<class T>
	static void InterpolateCubic(const T& v0, const T& v1, const T& v2, const T& v3, double t, T& result);
	
};

