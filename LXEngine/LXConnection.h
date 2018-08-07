//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXSmartObject.h"

class LXConnector;

class LXCORE_API LXConnection : public LXSmartObject
{

public:

	LXConnection();
	LXConnection(LXConnector* source, LXConnector* destination);
	virtual ~LXConnection();

private:

	void DefineProperties();

public:

	shared_ptr<LXConnector> Source;
	shared_ptr<LXConnector> Destination;
};

