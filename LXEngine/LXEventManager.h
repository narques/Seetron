//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXEvent.h"

class LXCORE_API LXEventManager
{

public:

	LXEventManager();
	~LXEventManager();
	
	void RegisterEvent(EEventType EventType, LXActor* Actor);
	void RegisterEvent(EEventType EventType, std::function<void(LXEvent*)> Func);
	
	// Immediate broadcast
	void BroadCastEvent(EEventType EventType);
	void BroadCastEvent(LXEvent* Event);

	// Deferred broadcast system
	// Useful to send event outside of the MainThread
	//void PostEvent(EEventType EventType);
	void PostEvent(LXEvent* Event);
	void BroadCastEvents();

private:

	void BroadCastEvent(LXActor* Actor, LXEvent* Event);

private:

	// Predefined Actor Callbacks. See LXActor.h
	map < EEventType, set<LXActor*>> EventActors;

	// Simple callbacks.
	map < EEventType, list<std::function<void(LXEvent*)>>> EventFunctions;

	// 
	set < LXEvent* > EventDeferred; 
	
};

