//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXVec3.h"

enum class EEventType			// Known emitters
{
	// Event
	SelectionChanged,			// SelectionManager

	ProjectLoading,
	ProjectLoaded,				// ProjectManager  ( EventResult )
	ProjectClosed,

	// Specialized Event
	MouseMoveOnActor,			// Viewport
	MouseLButtonDownOnActor,	// Viewport
	MouseLButtonUpOnActor,		// Viewport

	ConsoleCommandAdded,		// Console command is created and added to the ConsoleManager.
	ConsoleCommandDeleted,		// Console command removed from the ConsoleManager and deleted.
};

enum class EMouseButton
{
	None,
	Left,
	Middle,
	Right
};

class LXEvent
{
 public:
	LXEvent(EEventType EventType);
	virtual ~LXEvent() {};
	EEventType EventType;
};

class LXEventResult : public LXEvent
{
public:
	LXEventResult(EEventType EventType, bool InSuccess):LXEvent(EventType),Success(InSuccess) { }
	bool Success;
};

class LXEventMouse : public LXEvent
{
public:
	LXEventMouse(EMouseButton MouseButton);
};

class LXEvenLButtonDown : public LXEvent
{
public:
	LXEvenLButtonDown(const vec3f& v);
};

class LXEvenLButtonUp : public LXEvent
{
public:
	LXEvenLButtonUp(const vec3f& v);
};

class  LXEventObjectCreated : public LXEvent
{
public:
	LXEventObjectCreated(EEventType EventType, LXObject* object):LXEvent(EventType), CreatedObject(object) { }
	LXObject* CreatedObject;
};

class  LXEventObjectDeleted : public LXEvent
{
public:
	LXEventObjectDeleted(EEventType EventType, LXObject* object) :LXEvent(EventType), DeleteObject(object) { }
	LXObject* DeleteObject;
};
