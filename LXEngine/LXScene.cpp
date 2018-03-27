//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "StdAfx.h"
#include "LXScene.h"
#include "LXActorCamera.h"
#include "LXTraverserCallBack.h"
#include "LXMemory.h" // --- Must be the last included ---

LXScene::LXScene(LXProject* pDocument):
LXActor(pDocument)
{
  SetName(L"Scene");
}

LXScene::~LXScene(void)
{
	CHK(_MapCBOnActorAdded.size() == 0);
	CHK(_MapCBOnActorRemoved.size() == 0);
}

LXActor* LXScene::GetActor(const LXString& Name)
{
	LXActor* Result = nullptr;
	LXTraverserLambda TraverserLambda(this);
	TraverserLambda.SetLambdaOnGroup([this, &Name, &Result](LXActor* Actor)
	{
		if (Actor->GetName() == Name)
		{
			Result = Actor;
		}
	});

	TraverserLambda.Apply();
	return Result;
}

void LXScene::RegisterCB_OnActorAdded(void* listener, std::function<void(LXActor*)> func)
{
	_MapCBOnActorAdded[listener] = func;
}

void LXScene::UnregisterCB_OnActorAdded(void* listener)
{
	_MapCBOnActorAdded.erase(listener);
}

void LXScene::RegisterCB_OnActorRemoved(void* listener, std::function<void(LXActor*)> func)
{
	_MapCBOnActorRemoved[listener] = func;
}

void LXScene::UnregisterCB_OnActorRemoved(void* listener)
{
	_MapCBOnActorRemoved.erase(listener);
}

void LXScene::OnActorAdded(LXActor* Actor)
{
	if (LXActorCamera* ActorCamera = dynamic_cast<LXActorCamera*>(Actor))
	{
		_ActorCamera = ActorCamera;
	}
	
	for (auto It : _MapCBOnActorAdded)
	{
		It.second(Actor);
	}
}

void LXScene::OnActorRemoved(LXActor* Actor)
{
	if (LXActorCamera* ActorCamera = dynamic_cast<LXActorCamera*>(Actor))
	{
		_ActorCamera = nullptr;
	}

	for (auto It : _MapCBOnActorRemoved)
	{
		It.second(Actor);
	}
}

