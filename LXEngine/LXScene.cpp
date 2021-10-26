//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "pch.h"
#include "LXScene.h"
#include "LXActorCamera.h"
#include "LXEngine.h"
#include "LXEvent.h"
#include "LXEventManager.h"
#include "LXTraverserCallBack.h"

LXScene::LXScene(LXProject* pDocument):
LXActor(pDocument)
{
	SetName(L"Scene");
}

LXScene::~LXScene(void)
{
}

LXActor* LXScene::GetActor(const LXString& name)
{
	LXActor* result = nullptr;
	LXTraverserLambda TraverserLambda(this);
	TraverserLambda.SetLambdaOnGroup([this, &name, &result](LXActor* Actor)
	{
		if (Actor->GetName() == name)
		{
			result = Actor;
		}
	});

	TraverserLambda.Apply();
	return result;
}

void LXScene::RegisterCB_OnActorAdded(void* listener, std::function<void(LXEvent*)> func)
{
	GetEventManager()->RegisterEventFunc(EEventType::ActorAdded, listener, func);
}

void LXScene::UnregisterCB_OnActorAdded(void* listener)
{
	GetEventManager()->UnregisterEventFunc(EEventType::ActorAdded, listener);
}

void LXScene::RegisterCB_OnActorRemoved(void* listener, std::function<void(LXEvent*)> func)
{
	GetEventManager()->RegisterEventFunc(EEventType::ActorRemoved, listener, func);
}

void LXScene::UnregisterCB_OnActorRemoved(void* listener)
{
	GetEventManager()->UnregisterEventFunc(EEventType::ActorRemoved, listener);
}

void LXScene::OnLoaded()
{
	for (LXActor* actor : _actors)
		actor->SceneLoaded();
}

void LXScene::OnActorAdded(LXActor* actor)
{
	if (LXActorCamera* ActorCamera = dynamic_cast<LXActorCamera*>(actor))
	{
		_actorCamera = ActorCamera;
	}

	_actors.push_back(actor);
	GetEventManager()->PostEvent(new LXEventObjectCreated(EEventType::ActorAdded, actor));
}

void LXScene::OnActorRemoved(LXActor* actor)
{
	if (LXActorCamera* ActorCamera = dynamic_cast<LXActorCamera*>(actor))
	{
		_actorCamera = nullptr;
	}

	_actors.remove(actor);
	GetEventManager()->PostEvent(new LXEventObjectDeleted(EEventType::ActorRemoved, actor));
}
