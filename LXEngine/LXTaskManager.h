//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXObject.h"

class LXTask;

class LXTaskManager : public LXObject
{
	 
public:

	LXTaskManager();
	virtual ~LXTaskManager();

	void Run(float deltaTime);
 
	void EnqueueTask(LXTask* task);

	bool HasTasks() const { return _tasks.size() > 0; }
		
private:

	LXMutex* _mutex;
	list<LXTask*> _tasks;

 };
