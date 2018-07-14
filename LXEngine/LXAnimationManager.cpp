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
#include "LXMemory.h" // --- Must be the last included ---

LXAnimationManager::LXAnimationManager(const LXProject* pDocument)
{
	SetName(L"Animations");

	LXAnimation* pAnim = new LXAnimation();
	AddAnimation(pAnim);
}

LXAnimationManager::~LXAnimationManager(void)
{
	for (LXAnimation* animation : _listAnimations)
		delete animation;

	CHK(_listVolatileAnimations.size() == 0);
}

bool LXAnimationManager::OnSaveChild(const TSaveContext& saveContext) const
{
	for (LXAnimation* pAnimation : _listAnimations)
	{
		pAnimation->Save(saveContext);
	}
	return true;
}

bool LXAnimationManager::OnLoadChild(const TLoadContext& loadContext)
{
	if (loadContext.node.name() == L"LXAnimation")
	{
		LXAnimation* animation = *_listAnimations.begin();
		animation->Load(loadContext);
	}
	else
	{
		auto str = loadContext.node.name();
		CHK(0);
	}

	return true;
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

		_activeAnimation = *_listAnimations.begin();
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
		
		_activeAnimation = NULL; 
	}
}

bool LXAnimationManager::Update(double dFrameTime)
{
	bool bRet = false;

	if (GetCore().GetPlayMode())
	{
		for (auto It : _setSceneObjectToUpdate)
		{
			It->RunRT(dFrameTime);
		}

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
	
	if (GetCore().GetPlayMode())
	{
		bRet |= _setSceneObjectToUpdate.size() ? true : false;
		bRet |= _activeAnimation ? true : false;
	}
	
	return bRet;
}

void LXAnimationManager::AddAnimation(LXAnimation* pAnimation)
{
	CHK(pAnimation);
	if (!pAnimation)
		return;

	_listAnimations.push_back(pAnimation);
}

void LXAnimationManager::PlayVolatileAnimation(LXAnimation* pAnimation)
{
	CHK(pAnimation);
	if (!pAnimation)
		return;

	_listVolatileAnimations.push_back(pAnimation);
}

void LXAnimationManager::AddBehavior(LXActor* pGroup)
{
	_setSceneObjectToUpdate.insert(pGroup);
}

void LXAnimationManager::RemoveBehavior(LXActor* pGroup)
{
	_setSceneObjectToUpdate.erase(pGroup);
}

LXAnimation* LXAnimationManager::GetAnimation()
{
	return *_listAnimations.begin();
}

void LXAnimationManager::GetChildren(ListSmartObjects& list)
{
	for (LXAnimation* anim : _listAnimations)
		list.push_back(anim);
}     

void LXAnimationManager::Remove(LXKey* pKey)
{
	GetAnimation()->Remove(pKey);
}

void LXAnimationManager::Add(LXKey* pKey)
{
	GetAnimation()->Add(pKey);
}
