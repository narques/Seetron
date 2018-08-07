//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

class LXObjectFactory
{
public:

	LXObjectFactory();
	~LXObjectFactory();

	static LXSmartObject* CreateObject(const LXString& objectClassName, LXSmartObject* owner);
};

