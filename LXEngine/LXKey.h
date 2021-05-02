//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXSmartObject.h"

class LXSmartObject;
class LXTrack;

class LXKey : public LXSmartObject
{

public:

	LXKey(LXTrack* pTrack, LXVariant* Variant);
	virtual ~LXKey();

	void SetVariant(LXVariant* p) { CHK(p); _pVariant = p; }
	LXVariant* GetVariant() const { CHK(_pVariant); 	return _pVariant; }

	LXTrack* GetTrack() const { return _pTrack; }

private:

	void DefineProperties();

protected:

	GetSetDef(int, _time, Time, 0);
	LXVariant*	_pVariant;
	LXTrack*	_pTrack;

};

typedef std::map <int, LXKey*> MapKeys;
