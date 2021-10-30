//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXCore/LXObject.h"

class LXMutex;
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
	std::list<LXTask*> _tasks;

 };
