//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "pch.h"
#include "LXTask.h"
#include "LXTaskManager.h"
#include "LXEngine.h"

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
