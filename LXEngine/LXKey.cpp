//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "pch.h"
#include "LXKey.h"

LXKey::LXKey(LXTrack* pTrack, LXVariant* Variant):_pTrack(pTrack),_pVariant(Variant)
{
	SetName(L"Key");
	DefineProperties();
}

LXKey::~LXKey()
{
	LX_SAFE_DELETE(_pVariant);
}

void LXKey::DefineProperties()
{
	if (LXVariantT<vec3f>* v = dynamic_cast<LXVariantT<vec3f>*>(_pVariant))
		DefinePropertyVec3f(L"vec3f", LXPropertyID::KEY_VALUE, v->GetValuePtr());
	else if (LXVariantT<LXColor4f>* v = dynamic_cast<LXVariantT<LXColor4f>*>(_pVariant))
		DefinePropertyColor4f(L"color", LXPropertyID::KEY_VALUE, v->GetValuePtr());
	else if (LXVariantT<float>* v = dynamic_cast<LXVariantT<float>*>(_pVariant))
		DefinePropertyFloat(L"float", LXPropertyID::KEY_VALUE, v->GetValuePtr());
	else
		CHK(0);

	DefinePropertyInt(L"Time", LXPropertyID::KEY_TIME, &_time);
}
 
