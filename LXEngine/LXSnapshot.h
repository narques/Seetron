//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXSmartObject.h"

class LXProject;

class LXCORE_API LXSnapshot : public LXSmartObject
{

public:

	LXSnapshot();
	virtual ~LXSnapshot();

	void AddProperty(LXProperty* pProperty);
	void CaptureCurrentCamera(LXProject* InProject);

private:

	void DefineProperties();

public:

	GetSet(LXString, _strDescription, Description);
	
private:

	ListProperties			_listProperties;

};

typedef list<LXSnapshot*> ListSnapshots;