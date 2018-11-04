//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXVec3.h"

enum class EEventType			// Known emitters
{
	// Event
	SelectionChanged,			// SelectionManager

	ProjectLoaded,				// ProjectManager  ( EventResult )
	ProjectClosed,

	// Specialized Event
	MouseMoveOnActor,			// Viewport
	MouseLButtonDownOnActor,	// Viewport
	MouseLButtonUpOnActor,		// Viewport

	ConsoleCommandAdded,		// A new console command was added to the ConsoleManager.

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

