//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "LXActor.h"
#include "LXEventManager.h"
#include "LXMutex.h"
#include "LXThreadManager.h"

LXEventManager::LXEventManager()
{
	_mutex = new LXMutex();
}

LXEventManager::~LXEventManager()
{
	CHK(EventActors.size() == 0);
	CHK(_eventTypeFunctions.size() == 0);
	CHK(EventDeferred.size() == 0);
	delete _mutex;
}

void LXEventManager::RegisterEvent(EEventType Event, LXActor* Actor)
{
	EventActors[Event].insert(Actor);
}

void LXEventManager::RegisterEventFunc(EEventType Event, void* Owner, std::function<void(LXEvent*)> Func)
{
	_eventTypeFunctions[Event].push_back(std::pair<void*, std::function<void(LXEvent*)>>(Owner, Func));
}


void LXEventManager::RegisterEventFunc(const LXString& eventName, void* Owner, std::function<void(LXEvent*)> function)
{
	GetCurrentThreadId();
	_eventNameFunctions[eventName].push_back(std::pair<void*, std::function<void(LXEvent*)>>(Owner, function));
}

void LXEventManager::UnregisterEvent(EEventType EventType, LXActor* Actor)
{
	CHK(EventActors.find(EventType) != EventActors.end());
	CHK(EventActors[EventType].find(Actor) != EventActors[EventType].end());
	EventActors[EventType].erase(Actor);
	if (EventActors[EventType].size() == 0)
	{
		EventActors.erase(EventType);
	}
}

void LXEventManager::UnregisterEventFunc(EEventType EventType, void* Owner)
{
	auto itEvent = _eventTypeFunctions.find(EventType);
	CHK(itEvent != _eventTypeFunctions.end());
	
	auto& list = _eventTypeFunctions[EventType];

	// Find the pair using the Owner
	auto itPair = std::find_if(list.begin(), list.end(), [Owner](std::pair<void*, std::function<void(LXEvent*)>>& element)
	{ 
		return element.first == Owner;
	});

	CHK(itPair != list.end());
	
	list.erase(itPair);

	if (list.size() == 0)
	{
		_eventTypeFunctions.erase(itEvent);
	}
}

void LXEventManager::UnregisterEventFunc(const LXString& eventName, void* Owner)
{
	auto itEvent = _eventNameFunctions.find(eventName);
	CHK(itEvent != _eventNameFunctions.end());

	auto& list = _eventNameFunctions[eventName];

	// Find the pair using the Owner
	auto itPair = std::find_if(list.begin(), list.end(), [Owner](std::pair<void*, std::function<void(LXEvent*)>>& element)
	{
		return element.first == Owner;
	});

	CHK(itPair != list.end());

	list.erase(itPair);

	if (list.size() == 0)
	{
		_eventNameFunctions.erase(itEvent);
	}
}

void LXEventManager::BroadCastEvent(EEventType Event)
{
	BroadCastEvent(new LXEvent(Event));
}

void LXEventManager::BroadCastEvent(const LXString& eventName)
{
	//LXEvent* event = LXEvent(Event);

	auto It3 = _eventNameFunctions.find(eventName);
	if (It3 != _eventNameFunctions.end())
	{
		for (auto Func : It3->second)
		{
			Func.second(nullptr/*Event*/);
		}
	}
	//delete Event;
}


void LXEventManager::BroadCastEvent(LXEvent* Event)
{
	CHK(IsMainThread());

	auto It = EventActors.find(Event->EventType);
	if (It != EventActors.end())
	{
		for (LXActor* Actor : It->second)
		{
			BroadCastEvent(Actor, Event);
		}
	}

	auto It2 = _eventTypeFunctions.find(Event->EventType);
	if (It2 != _eventTypeFunctions.end())
	{
		for (auto Func : It2->second)
		{
			Func.second (Event);
		}
	}
	
	delete Event;
}

void LXEventManager::BroadCastEvent(LXActor* Actor, LXEvent* Event)
{
	CHK(Actor);

	switch (Event->EventType)
	{
		case EEventType::MouseMoveOnActor: 
		{
			// This Event is only sent to the "respective" Actor
			//if (Actor == Event->)
			{
				Actor->OnMouseMove();
			}
		}
		break;

		case EEventType::MouseLButtonDownOnActor: 
		{
			// This Event is only sent to the "respective" Actor
			//if (Actor == Event->)
			{
				Actor->OnMouseButtonDown();
			}
		}
		break;
		
		case EEventType::MouseLButtonUpOnActor: 
		{
			// This Event is only sent to the "respective" Actor
			//if (Actor == Event->)
			{
				Actor->OnMouseButtonUp();
			}
		}
		break;

		case EEventType::SelectionChanged: const_cast<LXActor*>(Actor)->OnSelectionChanged(); break;


		default: CHK(0); break;
	}
}

void LXEventManager::PostEvent(EEventType eventType)
{
	PostEvent(new LXEvent(eventType));
}

void LXEventManager::PostEvent(LXEvent* event)
{
	_mutex->Lock();
	EventDeferred.insert(event);
	_mutex->Unlock();
}

void LXEventManager::PostEvent(const LXString& eventName)
{
	_mutex->Lock();
	eventNameDeferred.insert(eventName);
	_mutex->Unlock();
}

void LXEventManager::BroadCastEvents()
{
	_mutex->Lock();
	for (LXEvent* event : EventDeferred)
	{
		BroadCastEvent(event);
	}

	EventDeferred.clear();

	for (const LXString& eventName : eventNameDeferred)
	{
		BroadCastEvent(eventName);
	}

	eventNameDeferred.clear();
	_mutex->Unlock();
}

