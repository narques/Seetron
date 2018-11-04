//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "LXActor.h"
#include "LXEventManager.h"
#include "LXThread.h"
#include "LXMemory.h" // --- Must be the last included ---

LXEventManager::LXEventManager()
{
}

LXEventManager::~LXEventManager()
{
	CHK(EventActors.size() == 0);
	CHK(EventFunctions.size() == 0);
	CHK(EventDeferred.size() == 0);
}

void LXEventManager::RegisterEvent(EEventType Event, LXActor* Actor)
{
	EventActors[Event].insert(Actor);
}

void LXEventManager::RegisterEventFunc(EEventType Event, void* Owner, std::function<void(LXEvent*)> Func)
{
	EventFunctions[Event].push_back(pair<void*, std::function<void(LXEvent*)>>(Owner, Func));
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
	auto itEvent = EventFunctions.find(EventType);
	CHK(itEvent != EventFunctions.end());
	
	auto& list = EventFunctions[EventType];

	// Find the pair using the Owner
	auto itPair = std::find_if(list.begin(), list.end(), [Owner](std::pair<void*, std::function<void(LXEvent*)>>& element)
	{ 
		return element.first == Owner;
	});

	CHK(itPair != list.end());
	
	list.erase(itPair);

	if (list.size() == 0)
	{
		EventFunctions.erase(itEvent);
	}
}

void LXEventManager::BroadCastEvent(EEventType Event)
{
	BroadCastEvent(new LXEvent(Event));
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

	auto It2 = EventFunctions.find(Event->EventType);
	if (It2 != EventFunctions.end())
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
	LXMutex Mutex;
	Mutex.Lock();
	EventDeferred.insert(event);
	Mutex.Unlock();
}

void LXEventManager::BroadCastEvents()
{
	for (LXEvent* Event : EventDeferred)
	{
		BroadCastEvent(Event);
	}

	EventDeferred.clear();
}

