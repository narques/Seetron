//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXObject.h"
#include "LXProperty.h"

class LXProperty;
typedef std::list<LXProperty*>	ListProperties;

class LXCORE_API LXPropertyManager : public LXObject
{

public:

	LXPropertyManager();
	virtual ~LXPropertyManager(void);

	// Creates unique ID : if the label already used, It returns -1
	static int CreatePropertyID(const LXString& label); 

	// Creates a ID or returns the exiting one.
	static int GetPropertyID(const LXString& label);

	static const LXString GetPropertyLabel(LXPropertyID id);
		
	static void GetProperties(const SetActors& setActor, const LXPropertyID& PID, ListProperties& olistProperties);

};

