//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "LXColor.h"
#include "LXString.h"

template<>
LXColor4<float>::LXColor4(const LXStringA& strHexa)
{
	SetHexa(strHexa);
	a = 1.f;
}

template<>
void LXColor4<float>::SetHexa(const LXStringA& str)
{
	LXStringA strCleaned = str;
	strCleaned.Replace("#", "");
	std::stringstream stream(strCleaned.GetBuffer());
	unsigned int val;
	stream >> std::hex >> val;
	r = ((val >> 16) & 0xFF) / 255.0f;
	g = ((val >> 8) & 0xFF) / 255.0f;
	b = ((val)& 0xFF) / 255.0f;
}

// Explicit class Instantiation
template class LXENGINE_API LXColor3<float>;
template class LXENGINE_API LXColor4<float>;
