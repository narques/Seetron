//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXActor.h"

typedef vector<class LXActorLight*> ArrayLights;
class LXActorCamera;
class LXEvent;

class LXCORE_API LXScene : public LXActor
{

	friend class LXActor;

public:

	LXScene(LXProject* project);
	virtual ~LXScene(void);

	// Returns the actor of the given name (case sensitive)
	LXActor* GetActor(const LXString& name);

	// Returns the current active Scene Camera
	// null if no camera exists.
	LXActorCamera* GetCamera() const { return _actorCamera; }

	void RegisterCB_OnActorAdded(void* listener, std::function<void(LXEvent*)>);
	void UnregisterCB_OnActorAdded(void* listener);

	void RegisterCB_OnActorRemoved(void* listener, std::function<void(LXEvent*)>);
	void UnregisterCB_OnActorRemoved(void* listener);

private:

	void OnActorAdded(LXActor* actor);
	void OnActorRemoved(LXActor* actor);

private:

	LXActorCamera* _actorCamera = nullptr;
};
