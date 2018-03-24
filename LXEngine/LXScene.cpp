//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "StdAfx.h"
#include "LXScene.h"
#include "LXTraverserCallBack.h"
#include "LXMemory.h" // --- Must be the last included ---


LXScene::LXScene(LXProject* pDocument):
LXActor(pDocument)
{
  SetName(L"Scene");
}

LXScene::~LXScene(void)
{
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

