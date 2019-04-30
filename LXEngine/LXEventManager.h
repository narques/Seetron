//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2019 Nicolas Arques. All rights reserved.
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
	void UnregisterEvent(EEventType EventType, LXActor* Actor);

	void RegisterEventFunc(EEventType EventType, void* Owner, std::function<void(LXEvent*)> Func);
	void UnregisterEventFunc(EEventType EventType, void* Owner);

	void RegisterEventFunc(const LXString& eventName, void* Owner, std::function<void(LXEvent*)> function);
	void UnregisterEventFunc(const LXString& eventName, void* Owner);

			
	// Immediate broadcast
	void BroadCastEvent(EEventType EventType);
	void BroadCastEvent(LXEvent* Event);
	void BroadCastEvent(const LXString& eventName);

	// Deferred broadcast system
	// Useful to send event outside of the MainThread
	void PostEvent(EEventType EventType);
	void PostEvent(LXEvent* Event);
	void PostEvent(const LXString& eventName);

	void BroadCastEvents();

private:

	void BroadCastEvent(LXActor* Actor, LXEvent* Event);

private:

	LXMutex* _mutex;

	// Predefined Actor Callbacks. See LXActor.h
	map < EEventType, set<LXActor*>> EventActors;

	// Simple callbacks.
	map < EEventType, list<pair<void*, std::function<void(LXEvent*)>>>> _eventTypeFunctions;
	map < LXString, list<pair<void*, std::function<void(LXEvent*)>>>> _eventNameFunctions;

	// 
	set < LXEvent* > EventDeferred; 
	set < LXString > eventNameDeferred;
	
};

