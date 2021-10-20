//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

// Simple Static Two-Dimensional Array 
template <class T, int X, int Y>
class LXArray2D
{
public:

	const T& GetAt(int x, int y) const
	{ 
		return Data[x][y];
	}

	T& GetAt(int x, int y)
	{
		return Data[x][y];
	}

	T Data[X][Y];
};