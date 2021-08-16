//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "LXMath.h"

int UpperPO2(int a)
{
	int rval = 1;
	while (rval < a)
		rval <<= 1;
	return rval;
}

void GenerateRandomVectors(int Count, std::vector<vec3f>& Vectors)
{
	CHK(Count > 0);
	CHK(Vectors.size() == 0);

	for (int i = 0; i < Count; ++i)
	{
		float x = RandomFloat() * 2.f - 1.f;
		float y = RandomFloat() * 2.f - 1.f;
		float z = RandomFloat() * 2.f - 1.f;
		vec3f v(x, y, z);
		v.Normalize();
		Vectors.push_back(v);
	}
}
