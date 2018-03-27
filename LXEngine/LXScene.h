//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXActor.h"

typedef vector<class LXActorLight*> ArrayLights;
class LXActorCamera;

class LXCORE_API LXScene : public LXActor
{

	friend class LXActor;

public:

	LXScene(LXProject* pDocument);
	virtual ~LXScene(void);

	// Returns the actor of the given name (case sensitive)
	LXActor* GetActor(const LXString& Name);

	// Returns the current active Scene Camera
	// null if no camera exists.
	LXActorCamera* GetCamera() const { return _ActorCamera; }

	void RegisterCB_OnActorAdded(void* Listener, std::function<void(LXActor*)>);
	void UnregisterCB_OnActorAdded(void* Listener);

	void RegisterCB_OnActorRemoved(void* Listener, std::function<void(LXActor*)>);
	void UnregisterCB_OnActorRemoved(void* Listener);

private:

	void OnActorAdded(LXActor* Actor);
	void OnActorRemoved(LXActor* Actor);

private:

	map<void*, std::function<void(LXActor*)>> _MapCBOnActorAdded;
	map<void*, std::function<void(LXActor*)>> _MapCBOnActorRemoved;

	LXActorCamera* _ActorCamera = nullptr;
};
