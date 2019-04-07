//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once
#include "LXObject.h"

class LXTask : public LXObject
{

public:

	LXTask();
	virtual ~LXTask();

	virtual void Run() = 0;
};

class LXTaskCallBack : public LXTask
{

public:

	LXTaskCallBack(std::function <void()> function);
	virtual ~LXTaskCallBack();

	virtual void Run() override;

private:

	std::function <void()> _function;
 
 };



