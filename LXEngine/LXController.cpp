//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "LXController.h"
#include "LXActor.h"
#include "LXActor.h"
#include "LXActorMesh.h"
#include "LXAssetType.h"
#include "LXCore.h"
#include "LXMutex.h"
#include "LXNode.h"
#include "LXProperty.h"
#include "LXRenderer.h"
#include "LXMemory.h" // --- Must be the last included ---

LXController::LXController()
{
	_mutex = new LXMutex();

	// Listen the properties

	LXSmartObject::RegisterCB_OnPropertiesChanged(this, [this](LXSmartObject* SmartObject, LXProperty* Property)
	{
		if (LXActor* Actor = dynamic_cast<LXActor*>(SmartObject))
		{
			if (LXPropertyAssetPtr* PropertyAsset = dynamic_cast<LXPropertyAssetPtr*>(Property))
			{
				if ((EAssetType)(PropertyAsset->GetUserData()) == EAssetType::Material)
				{
					Actor->InvalidateRenderState();
				}
			}
		}
	});
}

LXController::~LXController()
{
	LXSmartObject::UnregisterCB_OnPropertiesChanged(this);
	Purge();
	delete _mutex;;
}

void LXController::SetRenderer(LXRenderer* Renderer)
{
	_Renderer = Renderer; 
}

void LXController::Purge()
{
	_SetActorToUpdateRenderState.clear();
	_SetActorToDelete.clear();
	_SetActorToMove.clear();
	
	_SetActorToUpdateRenderState_RT.clear();
	_SetActorToDelete_RT.clear();
		
	for (LXRendererUpdate* RendererUpdate : _RendererUpdates)
	{
		delete RendererUpdate;
	}

	_RendererUpdates.clear();
}

void LXController::AddActorToUpdateRenderStateSet(LXActor* Actor)
{
	CHK(IsMainThread() || IsLoadingThread());
	_mutex->Lock();
	LogD(LXController, L"AddActorToUpdateRenderStateSet %s", Actor->GetName().GetBuffer());
	_SetActorToUpdateRenderState.insert(Actor);
	_mutex->Unlock();
}

void LXController::AddActorToDeleteSet(LXActor* Actor)
{
	CHK(IsMainThread());
	_SetActorToDelete.insert(Actor);
}

void LXController::ActorWorldMatrixChanged(LXActor* Actor)
{
	if (!Actor->IsVisible())
		return;
	
	//LogD(LXController, L"ActorWorldMatrixChanged %s", Actor->GetName().GetBuffer());
	
	// All thread can Transform an Actor
	DWORD hThread = ::GetCurrentThreadId();

	if (IsMainThread())
	{
		// Main Thread, Use the Sync point
		_SetActorToMove.insert(Actor);
	}
	else if (IsRenderThread())
	{
		// RT, Create the UpudateState ans Store in fot the next RT Frame. As we do not know when our this event happens
		//LXRendererUpdateMatrix For a NextFrame !
		AddRendererUpdateMatrix(Actor);
	}
	else if (IsLoadingThread())
	{
		// OR Nothing do no. Wait unil actor loading is completed ( TODO OnActorLoaded Event ? )
		_SetActorToMove.insert(Actor);
	}
	else
	{
		CHK(0);
	}
}

void LXController::AddRendererUpdateMatrix(LXActor* Actor)
{
	if (LXActorMesh* ActorMesh = dynamic_cast<LXActorMesh*>(Actor))
	{
		const TWorldPrimitives& WorldPrimitives = ActorMesh->GetAllPrimitives();
		for (const LXWorldPrimitive& WorldPrimitive : WorldPrimitives)
		{
			LXRendererUpdateMatrix* RendererUpdateMatrix = new LXRendererUpdateMatrix();
			RendererUpdateMatrix->PrimitiveInstance = WorldPrimitive.PrimitiveInstance;
			RendererUpdateMatrix->Matrix = WorldPrimitive.MatrixWorld;
			RendererUpdateMatrix->BBox = WorldPrimitive.BBoxWorld;
			_RendererUpdates.push_back(RendererUpdateMatrix);
		}
	}
}

void LXController::Run()
{
	if (!RenderThread && LXRenderer::gUseRenderThread)
		return;

	// Now the RenderTread is waiting
	// TODO CHK(RenderThread.IsWaiting());
	
	// All updates should be consumed.
	//CHK(RendererUpdates.size() == 0);
	
	// Actors to move
	if (_SetActorToMove.size())
	{
		for (LXActor* Actor : _SetActorToMove)
		{
			AddRendererUpdateMatrix(Actor);
		}
		_SetActorToMove.clear();
	}

	//
	// Actors
	//

	{
		// Lock, _SetActorToUpdateRenderState can be modified by the LoadingThread.
		_mutex->Lock();
		SetActors& Actors = _SetActorToUpdateRenderState;
		// Previous actors should be consumed
		CHK(_SetActorToUpdateRenderState_RT.size() == 0);
		_SetActorToUpdateRenderState_RT.insert(Actors.begin(), Actors.end());

		for (const LXActor* Actor : _SetActorToUpdateRenderState_RT)
		{
			if (LXActorMesh* actorMesh = dynamic_cast<LXActorMesh*>(const_cast<LXActor*>(Actor)))
			{
				actorMesh->GetAllPrimitives();
			}

			LogD(LXController, L"Synchronized: %s", Actor->GetName().GetBuffer());
		}
		
		Actors.clear();
		_mutex->Unlock();
	}

	//
	// Destroy Actors marked for delete
	//
	
	{
		SetActors& Actors = _SetActorToDelete;
		for (SetActors::iterator It = Actors.begin(); It != Actors.end();)
		{
			if ((*It)->IsRenderStateValid())
			{
				delete *It;
				It = Actors.erase(It);
			}
			else
				It++;
		}
	}
}
