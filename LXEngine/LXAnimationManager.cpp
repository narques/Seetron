//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "StdAfx.h"
#include "LXAnimationManager.h"
#include "LXProject.h"
#include "LXAnimation.h"
#include "LXActor.h"
#include "LXMSXMLNode.h"
#include "LXEventManager.h"
#include "LXSelectionManager.h"

LXAnimationManager::LXAnimationManager(const LXProject* pDocument)
{
	SetName(L"Animations");

	// Register Events.
	GetEventManager()->RegisterEventFunc(EEventType::SelectionChanged, this, [this](LXEvent* Event)
	{
		const SetSmartObjects& objects =  GetSelectionManager()->GetSelection();
		for (LXSmartObject* object : objects)
		{
			if (LXAnimation* animation = dynamic_cast<LXAnimation*>(object))
			{
				_animation = animation;
				return;
			}
		}
	});
}

LXAnimationManager::~LXAnimationManager(void)
{
	GetEventManager()->UnregisterEventFunc(EEventType::SelectionChanged, this);
	CHK(_listVolatileAnimations.size() == 0);
}

void LXAnimationManager::Play(bool bPlay)
{
	if (bPlay)
	{
		// Save SceneObject matrix
		for (auto It : _setSceneObjectToUpdate)
		{
			_mapMatrices[It] = It->GetMatrix();
		}

		_activeAnimation = _animation;
	}
	else
	{
		// Restore SceneObject matrix
		for (auto It : _setSceneObjectToUpdate)
		{
			CHK(0); // TODO: Finalize "Matrix To Transform"
			/*
			LXMatrix& mat = _mapMatrices[It];
			It->SetMatrix(mat);
			if (It->GetParent())
				It->GetParent()->InvalidateBounds(true);
			*/
		}
		
		_activeAnimation = nullptr; 
	}
}

bool LXAnimationManager::Update(double dFrameTime)
{
	bool bRet = false;

	if (GetEngine().GetPlayMode())
	{
		_activeAnimation->Update(dFrameTime);
	}
	
	// Volatile Animations
	
	ListAnimations toRemove;

	for (LXAnimation* pAnimation : _listVolatileAnimations)
	{
		if (pAnimation)
		{
			pAnimation->Update(dFrameTime);
			if (pAnimation->IsFinished())
			{
				delete pAnimation;
				toRemove.push_back(pAnimation);
			}
		}
		else
			CHK(0);
	}

	for (LXAnimation* pAnimation2 : toRemove)
	{
		_listVolatileAnimations.remove(pAnimation2);
	}
	 
	bRet = _listVolatileAnimations.size() ? true : false;
	
	if (GetEngine().GetPlayMode())
	{
		bRet |= _setSceneObjectToUpdate.size() ? true : false;
		bRet |= _activeAnimation ? true : false;
	}
	
	return bRet;
}

void LXAnimationManager::SetAnimation(LXAnimation* animation)
{
	_animation = animation;
}

void LXAnimationManager::PlayVolatileAnimation(LXAnimation* animation)
{
	CHK(animation);
	if (!animation)
		return;

	_listVolatileAnimations.push_back(animation);
}

void LXAnimationManager::AddBehavior(LXActor* actor)
{
	_setSceneObjectToUpdate.insert(actor);
}

void LXAnimationManager::RemoveBehavior(LXActor* actor)
{
	_setSceneObjectToUpdate.erase(actor);
}

LXAnimation* LXAnimationManager::GetAnimation()
{
	return _animation;
}

void LXAnimationManager::Remove(LXKey* pKey)
{
	GetAnimation()->Remove(pKey);
}

void LXAnimationManager::Add(LXKey* pKey)
{
	GetAnimation()->Add(pKey);
}
