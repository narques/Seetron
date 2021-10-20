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

class LXENGINE_API LXConnection : public LXSmartObject
{

public:

	LXConnection();
	LXConnection(LXConnector* source, LXConnector* destination);
	virtual ~LXConnection();
	void Detach(LXConnector* owner);

private:

	void DefineProperties();
	void OnLoaded() override;

public:

	LXReference<LXConnector> Source;
	LXReference<LXConnector> Destination;
};

