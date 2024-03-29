//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "pch.h"
#include "LXMutex.h"
#include "LXPerformance.h"
#include "LXTaskManager.h"
#include "LXTask.h"
 
LXTaskManager::LXTaskManager()
{
	_mutex = new LXMutex();
}
 
LXTaskManager::~LXTaskManager()
{
	delete _mutex;
}
 
void LXTaskManager::EnqueueTask(LXTask* task)
{
	_mutex->Lock();
	_tasks.push_back(task);
	_mutex->Unlock();
}
 
void LXTaskManager::Run(float deltaTime)
{
	LXPerformance performance;

	while (_tasks.size())
	{
		_mutex->Lock();
		LXTask* task = *_tasks.begin();
		_tasks.pop_front();
		_mutex->Unlock();
		
		task->Run();
		delete task;
	}
}


