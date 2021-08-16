//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

class LXSmartObject;

class LXObjectFactory
{
public:

	LXObjectFactory();
	~LXObjectFactory();

	static LXSmartObject* CreateObject(const LXString& objectClassName, LXSmartObject* owner);
	static std::shared_ptr<LXSmartObject> CreateSharedObject(const LXString& objectClassName, LXSmartObject* owner);
};

