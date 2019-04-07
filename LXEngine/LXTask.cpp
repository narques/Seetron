//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "LXTask.h"
#include "LXTaskManager.h"
#include "LXCore.h"

LXTask::LXTask()
{
}
 
LXTask::~LXTask()
{
}
 
LXTaskCallBack::LXTaskCallBack(std::function <void()> function):_function(function)
{
}

LXTaskCallBack::~LXTaskCallBack()
{

}

void LXTaskCallBack::Run()
{
	_function();
}
