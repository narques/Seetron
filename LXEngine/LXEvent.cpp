//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "pch.h"
#include "LXEvent.h"

LXEvent::LXEvent(EEventType EventType) : EventType(EventType)
{
}

LXEventMouse::LXEventMouse(EMouseButton MouseButton) : LXEvent(EEventType::MouseMoveOnActor)
{
}

LXEvenLButtonDown::LXEvenLButtonDown(const vec3f& Point) : LXEvent(EEventType::MouseLButtonDownOnActor)
{
}

LXEvenLButtonUp::LXEvenLButtonUp(const vec3f& v) : LXEvent(EEventType::MouseLButtonUpOnActor)
{
}