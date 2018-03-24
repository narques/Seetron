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
}

void LXEventManager::RegisterEvent(EEventType Event, LXActor* Actor)
{
	EventActors[Event].insert(Actor);
}

void LXEventManager::RegisterEvent(EEventType Event, std::function<void(LXEvent*)> Func)
{
	EventFunctions[Event].push_back(Func);
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
			Func(Event);
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

		case EEventType::SelectionChanged: Actor->OnSelectionChanged(); break;


		default: CHK(0); break;
	}
}

// void LXEventManager::PostEvent(EEventType Event)
// {
// 	PostEvent(new LXEvent(Event));
// }

void LXEventManager::PostEvent(LXEvent* Event)
{
	LXMutex Mutex;
	Mutex.Lock();
	EventDeferred.insert(Event);
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

